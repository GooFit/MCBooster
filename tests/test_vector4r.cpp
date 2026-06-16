// Exercises the Lorentz-vector math that user kinematics code (and GooFit's
// Amp3Body/Amp4Body PDFs) relies on. Pure host code -- no Thrust execution.
#include <catch2/catch.hpp>

#include <mcbooster/Vector4R.h>

using mcbooster::GReal_t;
using mcbooster::Vector4R;

TEST_CASE("Vector4R component access", "[vector4r]") {
    Vector4R v(5.0, 1.0, 2.0, 3.0);
    CHECK(v.get(0) == Approx(5.0));
    CHECK(v.get(1) == Approx(1.0));
    CHECK(v.get(2) == Approx(2.0));
    CHECK(v.get(3) == Approx(3.0));

    v.set(2, 9.0);
    CHECK(v.get(2) == Approx(9.0));

    v.set(7.0, 0.0, 0.0, 1.0);
    CHECK(v.get(0) == Approx(7.0));
    CHECK(v.get(3) == Approx(1.0));
}

TEST_CASE("Vector4R invariant mass", "[vector4r]") {
    // E^2 - |p|^2 = 25 - 9 = 16
    Vector4R v(5.0, 1.0, 2.0, 2.0);
    CHECK(v.mass2() == Approx(16.0));
    CHECK(v.mass() == Approx(4.0));
}

TEST_CASE("Vector4R Minkowski dot product", "[vector4r]") {
    Vector4R a(5.0, 1.0, 2.0, 2.0);
    Vector4R b(3.0, 1.0, 0.0, 1.0);

    // operator* is the Minkowski product: t1*t2 - (x1*x2 + y1*y2 + z1*z2)
    //   = 15 - (1 + 0 + 2) = 12
    CHECK((a * b) == Approx(12.0));
    CHECK((a * a) == Approx(a.mass2()));

    // dot() is the spatial 3-vector product: x1*x2 + y1*y2 + z1*z2 = 1 + 0 + 2
    CHECK(a.dot(b) == Approx(3.0));
}

TEST_CASE("Vector4R addition and subtraction", "[vector4r]") {
    Vector4R a(5.0, 1.0, 2.0, 2.0);
    Vector4R b(3.0, 1.0, 0.0, 1.0);

    Vector4R sum = a + b;
    CHECK(sum.get(0) == Approx(8.0));
    CHECK(sum.get(1) == Approx(2.0));
    CHECK(sum.get(2) == Approx(2.0));
    CHECK(sum.get(3) == Approx(3.0));

    Vector4R diff = a - b;
    CHECK(diff.get(0) == Approx(2.0));
    CHECK(diff.get(1) == Approx(0.0));
    CHECK(diff.get(2) == Approx(2.0));
    CHECK(diff.get(3) == Approx(1.0));
}

TEST_CASE("Lorentz boost preserves invariant mass", "[vector4r]") {
    // applyBoostTo(p4) boosts by p4's velocity (beta = p/E); any such boost
    // leaves the Minkowski norm of the boosted vector unchanged.
    Vector4R frame(5.0, 1.0, 2.0, 2.0); // mass 4
    Vector4R p(4.0, 1.0, 1.0, 1.0);     // mass^2 = 16 - 3 = 13

    GReal_t m2_before = p.mass2();
    p.applyBoostTo(frame);
    CHECK(p.mass2() == Approx(m2_before));

    // The inverse boost by a vector's own velocity returns it to rest:
    // momentum -> 0 and energy -> the rest mass.
    Vector4R q(5.0, 1.0, 2.0, 2.0);
    q.applyBoostTo(frame, /*inverse=*/true);
    CHECK(q.get(1) == Approx(0.0).margin(1e-12));
    CHECK(q.get(2) == Approx(0.0).margin(1e-12));
    CHECK(q.get(3) == Approx(0.0).margin(1e-12));
    CHECK(q.get(0) == Approx(frame.mass()));
}
