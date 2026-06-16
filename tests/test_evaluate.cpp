// Exercises EvaluateArray with a user IFunctionArray functor -- the exact
// pattern GooFit uses to compute kinematic variables (e.g. Dim5) in parallel
// over a generated sample.
#include <catch2/catch.hpp>

#include <vector>

#include <mcbooster/EvaluateArray.h>
#include <mcbooster/GContainers.h>
#include <mcbooster/GFunctional.h>
#include <mcbooster/Generate.h>
#include <mcbooster/Vector4R.h>

using mcbooster::GInt_t;
using mcbooster::GReal_t;
using mcbooster::Vector4R;

namespace {
// Computes the three two-body invariant masses of a 3-body final state.
struct InvariantMasses : public mcbooster::IFunctionArray {
    InvariantMasses() { dim = 3; }

    __host__ __device__ void operator()(const GInt_t, Vector4R **p, GReal_t *out) override {
        out[0] = (*p[0] + *p[1]).mass();
        out[1] = (*p[0] + *p[2]).mass();
        out[2] = (*p[1] + *p[2]).mass();
    }
};

const GReal_t kMotherMass = 1.86484;
const std::vector<GReal_t> kMasses{0.493677, 0.139570, 0.134977};
const mcbooster::GLong_t kNEvents = 2000;
} // namespace

TEST_CASE("EvaluateArray matches a host-side recomputation", "[evaluate]") {
    mcbooster::PhaseSpace phsp(kMotherMass, kMasses, kNEvents);
    phsp.Generate(Vector4R(kMotherMass, 0.0, 0.0, 0.0));

    mcbooster::ParticlesSet_d particles{&phsp.GetDaughters(0), &phsp.GetDaughters(1), &phsp.GetDaughters(2)};

    mcbooster::RealVector_h m01(kNEvents);
    mcbooster::RealVector_h m02(kNEvents);
    mcbooster::RealVector_h m12(kNEvents);
    mcbooster::VariableSet_h variables{&m01, &m02, &m12};

    InvariantMasses functor;
    mcbooster::EvaluateArray<InvariantMasses>(functor, particles, variables);

    // Recompute on the host straight from the daughter four-vectors.
    mcbooster::Particles_h d0 = phsp.GetDaughters(0);
    mcbooster::Particles_h d1 = phsp.GetDaughters(1);
    mcbooster::Particles_h d2 = phsp.GetDaughters(2);

    const GReal_t lo01 = kMasses[0] + kMasses[1];
    const GReal_t hi01 = kMotherMass - kMasses[2];

    for(int i = 0; i < kNEvents; i++) {
        INFO("event " << i);
        CHECK(m01[i] == Approx((d0[i] + d1[i]).mass()).epsilon(1e-6));
        CHECK(m02[i] == Approx((d0[i] + d2[i]).mass()).epsilon(1e-6));
        CHECK(m12[i] == Approx((d1[i] + d2[i]).mass()).epsilon(1e-6));

        // And the invariant mass must respect the Dalitz boundaries.
        CHECK(m01[i] >= Approx(lo01).epsilon(1e-5));
        CHECK(m01[i] <= Approx(hi01).epsilon(1e-5));
    }
}
