#include "../TestHeader.h"
#include "../../Core/Random.h"

#include <algorithm>
#include <cmath>

CATCH_TEST_CASE("Test Random.h")
{
    Random::setSeed(0);
    
    CATCH_REQUIRE(Random::uniformInt(5,10) == 8);

    int imin = 10, imax = 0;
    double dmin = 10.0, dmax = 0.0;

    double mean = 0.0, var = 0.0;
    double norm[1000];

    for (unsigned i = 0; i < 1000; ++i)
    {
        imin = std::min(Random::uniformInt(0,10), imin);
        imax = std::max(Random::uniformInt(0,10), imax);

        dmin = std::min(Random::uniform(0,10), dmin);
        dmax = std::max(Random::uniform(0,10), dmax);

        norm[i] = Random::normal(0, 1);        
        mean += norm[i];
    }
        
    CATCH_REQUIRE(imin == 0);
    CATCH_REQUIRE(imax == 10);
    
    CATCH_REQUIRE(dmin < 0.1);
    CATCH_REQUIRE(dmax > 10 - 0.1);

    mean /= 1000;
    for (unsigned i = 0; i < 1000; ++i)
    {
        var += pow(norm[i] - mean, 2);
    }
    var /= 1000;
    CATCH_REQUIRE(mean == Approx(0).epsilon(0.025));
    CATCH_REQUIRE(var == Approx(1).epsilon(0.025));
}


