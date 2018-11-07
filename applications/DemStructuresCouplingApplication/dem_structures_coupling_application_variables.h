/*
 * Author: Miguel Angel Celigueta
 *
 *  maceli@cimne.upc.edu
 */

#ifndef KRATOS_DEM_STRUCTURES_COUPLING_APPLICATION_VARIABLES_H
#define KRATOS_DEM_STRUCTURES_COUPLING_APPLICATION_VARIABLES_H

#include "includes/define.h"
#include "includes/variables.h"
#include "DEM_application_variables.h"
#include "structural_mechanics_application_variables.h"

namespace Kratos {

    // STRUCTURAL COUPLING
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_STRUCTURES_COUPLING_APPLICATION, DEM_SURFACE_LOAD)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(DEM_STRUCTURES_COUPLING_APPLICATION, CURRENT_STRUCTURAL_VELOCITY)
    
    class DemStructuresCouplingFlags {

        public:
    };
}

#endif	/* KRATOS_DEM_STRUCTURES_COUPLING_APPLICATION_VARIABLES_H */
