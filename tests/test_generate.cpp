// Exercises the core phase-space generator (PhaseSpace::Generate) that GooFit
// uses to produce normalization/toy samples. Runs on whichever Thrust backend
// the build selects.
#include <catch2/catch.hpp>

#include <cmath>
#include <vector>

#include <mcbooster/GContainers.h>
#include <mcbooster/Generate.h>
#include <mcbooster/Vector4R.h>

using mcbooster::GReal_t;
using mcbooster::PhaseSpace;
using mcbooster::Vector4R;

namespace {
// D0 -> K- pi+ pi0, a realistic 3-body decay.
const GReal_t kMotherMass = 1.86484;
const std::vector<GReal_t> kMasses{0.493677, 0.139570, 0.134977};
const mcbooster::GLong_t kNEvents = 5000;
} // namespace

TEST_CASE("PhaseSpace reports its configuration", "[generate]") {
    PhaseSpace phsp(kMotherMass, kMasses, kNEvents);
    CHECK(phsp.GetNDaughters() == 3);
    CHECK(phsp.GetNEvents() == kNEvents);
}

TEST_CASE("Generated daughters have the requested masses", "[generate]") {
    PhaseSpace phsp(kMotherMass, kMasses, kNEvents);
    phsp.Generate(Vector4R(kMotherMass, 0.0, 0.0, 0.0));

    for(int d = 0; d < 3; d++) {
        mcbooster::Particles_h daughters = phsp.GetDaughters(d);
        for(int i = 0; i < 50; i++) {
            INFO("daughter " << d << ", event " << i);
            CHECK(daughters[i].mass() == Approx(kMasses[d]).epsilon(1e-5));
        }
    }
}

TEST_CASE("Generation conserves energy and momentum", "[generate]") {
    PhaseSpace phsp(kMotherMass, kMasses, kNEvents);
    phsp.Generate(Vector4R(kMotherMass, 0.0, 0.0, 0.0));

    mcbooster::Particles_h d0 = phsp.GetDaughters(0);
    mcbooster::Particles_h d1 = phsp.GetDaughters(1);
    mcbooster::Particles_h d2 = phsp.GetDaughters(2);

    for(int i = 0; i < 50; i++) {
        Vector4R total = d0[i] + d1[i] + d2[i];
        INFO("event " << i);
        CHECK(total.get(0) == Approx(kMotherMass).epsilon(1e-5)); // energy -> mother mass
        CHECK(total.get(1) == Approx(0.0).margin(1e-5));          // px
        CHECK(total.get(2) == Approx(0.0).margin(1e-5));          // py
        CHECK(total.get(3) == Approx(0.0).margin(1e-5));          // pz
        CHECK(total.mass() == Approx(kMotherMass).epsilon(1e-5));
    }
}

TEST_CASE("Weights are finite and positive", "[generate]") {
    PhaseSpace phsp(kMotherMass, kMasses, kNEvents);
    phsp.Generate(Vector4R(kMotherMass, 0.0, 0.0, 0.0));

    mcbooster::RealVector_h weights = phsp.GetWeights();
    REQUIRE(weights.size() == static_cast<size_t>(kNEvents));

    GReal_t sum = 0.0;
    for(size_t i = 0; i < weights.size(); i++) {
        INFO("event " << i);
        REQUIRE(std::isfinite(weights[i]));
        REQUIRE(weights[i] > 0.0);
        sum += weights[i];
    }
    CHECK(sum > 0.0);
}

TEST_CASE("Unweighting keeps a subset of events", "[generate]") {
    PhaseSpace phsp(kMotherMass, kMasses, kNEvents);
    phsp.Generate(Vector4R(kMotherMass, 0.0, 0.0, 0.0));

    mcbooster::GULong_t kept = phsp.Unweight();
    CHECK(kept > 0);
    CHECK(kept <= static_cast<mcbooster::GULong_t>(kNEvents));
}
