#include <Rcpp.h>
#include <iostream>

#include "../TestHeader.h"
#include "../../Core/Random.h"
#include "../../CellModels/DrasdoHohmeModel.h"

CATCH_TEST_CASE("Test DrasdoHohmeModel - basic operations")
{
    Random::setSeed(0);

    Rcpp::Environment pkgEnv;
    pkgEnv = Rcpp::Environment::namespace_env("CancerInSilico");
    Rcpp::S4 rModel = pkgEnv.find("drugsInSystem");

    DrasdoHohmeModel cModel (&rModel);

    // test constructor
    CATCH_REQUIRE(cModel.size() == 100);
    cModel.recordPopulation();
    Rcpp::List cellList = cModel.getCellsAsList();
    Rcpp::NumericVector initCells = cellList(0);
    CATCH_REQUIRE(initCells.size() == 8 * 100);

    CellIterator it = cModel.begin();
    for (; it != cModel.end(); ++it)
    {
        CATCH_REQUIRE((*it).cycleLength() == 48);
        CATCH_REQUIRE(!cModel.checkOverlap(*it));
        CATCH_REQUIRE(!cModel.checkBoundary(*it));
        CATCH_REQUIRE(cModel.growthRate(*it) > 0);
        CATCH_REQUIRE(cModel.numNeighbors(*it) <= 6);
    }

    // simple trials
    it = cModel.begin();
    OffLatticeCell cell0 = *(it++);
    OffLatticeCell cell1 = *(it++);
    OffLatticeCell cell2 = *(it++);
    OffLatticeCell cell3 = *(it++);

    double rad = cell0.radius();
    CATCH_REQUIRE_NOTHROW(cModel.growth(cell0));
    CATCH_REQUIRE(cell0.radius() - rad > 0);

    Point<double> crds = cell1.coordinates();
    CATCH_REQUIRE_NOTHROW(cModel.translation(cell1));
    CATCH_REQUIRE(cell1.coordinates() != crds);

    double ang = cell2.axisAngle();
    CATCH_REQUIRE_NOTHROW(cModel.rotation(cell2));
    CATCH_REQUIRE(fabs(ang - cell2.axisAngle()) > 0);

    cell3.setAxisLength(sqrt(cell3.type().size() * 8));
    CATCH_REQUIRE_NOTHROW(cModel.deformation(cell3));  
    CATCH_REQUIRE(cell3.axisLength() > sqrt(cell3.type().size() * 8));

    // trial acceptance
    CATCH_REQUIRE(!cModel.acceptTrial(Energy(10,0), Energy(1,0), 5, 4));
    CATCH_REQUIRE(cModel.acceptTrial(Energy(10,0), Energy(1,0), 5, 5));
    CATCH_REQUIRE(!cModel.acceptTrial(Energy(1,0), Energy(2,0), 5, 5));
    CATCH_REQUIRE(!cModel.acceptTrial(Energy(1,0), Energy(1.1,0), 5, 5));
    CATCH_REQUIRE(cModel.acceptTrial(Energy(1,0), Energy(1.001,0), 5, 5));

    // calculating neighbors    
    CATCH_REQUIRE(cModel.numNeighbors(cell3) == 0);
    CATCH_REQUIRE(cModel.numNeighbors(cell0) == 1);

    // calculate Hamiltonian
    CATCH_REQUIRE(cModel.calculateHamiltonian(cell0).first
        == Approx(-0.26).epsilon(0.01));
    CATCH_REQUIRE(cModel.calculateHamiltonian(cell1).first
        == Approx(816.38).epsilon(0.01));
    CATCH_REQUIRE(cModel.calculateHamiltonian(cell2).first
        == Approx(-1.26).epsilon(0.01));
    CATCH_REQUIRE(cModel.calculateHamiltonian(cell3).first
        == Approx(0.0).epsilon(0.01));

    // attempt trial function
    CATCH_REQUIRE_NOTHROW(cModel.attemptTrial(cell0));
    CATCH_REQUIRE_NOTHROW(cModel.attemptTrial(cell1));
    CATCH_REQUIRE_NOTHROW(cModel.attemptTrial(cell2));
    CATCH_REQUIRE_NOTHROW(cModel.attemptTrial(cell3));

    // check mitosis
    CATCH_REQUIRE_NOTHROW(cModel.checkMitosis(cell0));
    CATCH_REQUIRE_NOTHROW(cModel.checkMitosis(cell1));    
    CATCH_REQUIRE_NOTHROW(cModel.checkMitosis(cell2));    
    CATCH_REQUIRE_NOTHROW(cModel.checkMitosis(cell3));        

    // check drugs
    CATCH_REQUIRE_NOTHROW(cModel.updateDrugs(0));
}

CATCH_TEST_CASE("Test DrasdoHohmeModel - top level functions")
{
    Random::setSeed(0);

    Rcpp::Environment pkgEnv;
    pkgEnv = Rcpp::Environment::namespace_env("CancerInSilico");
    Rcpp::S4 rModel = pkgEnv.find("drugsInSystem");

    DrasdoHohmeModel cModel (&rModel);

    CellIterator it = cModel.begin();
    OffLatticeCell cell0 = *(it++);
    OffLatticeCell cell1 = *(it++);
    OffLatticeCell cell2 = *(it++);
    OffLatticeCell cell3 = *(it++);

    // do trial
    CATCH_REQUIRE_NOTHROW(cModel.doTrial(cell0));
    CATCH_REQUIRE_NOTHROW(cModel.doTrial(cell1));
    CATCH_REQUIRE_NOTHROW(cModel.doTrial(cell2));
    CATCH_REQUIRE_NOTHROW(cModel.doTrial(cell3));
    
    // entire monte carlo step
    CATCH_REQUIRE_NOTHROW(cModel.oneMCStep());
    CATCH_REQUIRE_NOTHROW(cModel.oneMCStep());
    CATCH_REQUIRE_NOTHROW(cModel.oneMCStep());
    CATCH_REQUIRE(cModel.size() == 100);

    // entire time step
    CATCH_REQUIRE_NOTHROW(cModel.oneTimeStep(0));
    CATCH_REQUIRE_NOTHROW(cModel.oneTimeStep(0));
    CATCH_REQUIRE_NOTHROW(cModel.oneTimeStep(0));
}
