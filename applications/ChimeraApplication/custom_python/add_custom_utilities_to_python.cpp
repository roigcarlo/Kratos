//
// ==============================================================================
//  ChimeraApplication
//
//  License:         BSD License
//                   license: ChimeraApplication/license.txt
//
//  Main authors:    Aditya Ghantasala, https://github.com/adityaghantasala
//                   Navaneeth K Narayanan
//
// ==============================================================================
// System includes

// External includes
#include <boost/python.hpp>


// Project includes
#include "includes/define.h"
#include "processes/process.h"
#include "includes/model_part.h"
#include "custom_python/add_custom_utilities_to_python.h"

#include "spaces/ublas_space.h"
#include "linear_solvers/linear_solver.h"

//Processes

#include "custom_utilities/vtk_output.hpp"
#include "custom_utilities/quadtree_binary_cell.h"
#include "custom_utilities/quadtree_binary.h"
//#include "custom_utilities/multipoint_constraint_data.hpp"

#include "custom_utilities/interpolation_utility.h"
#include "custom_utilities/fractional_step_settings_for_chimera.h"
#include "custom_utilities/periodic_condition_utilities_for_chimera.h"


namespace Kratos
{

template< unsigned int TDim >
void AuxCreateFluidBoundaryFaces(InterpolationUtility<TDim>& ThisUtility, ModelPart& rBackground)
{
    std::string ConditionName("ChimeraFluidCouplingCondition2D");
    const Condition& rReferenceCondition = KratosComponents<Condition>::Get(ConditionName);
    ThisUtility.CreateBoundaryFaces(rBackground,rReferenceCondition);
}


template< unsigned int TDim >
void AuxCreateThermalBoundaryFaces(InterpolationUtility<TDim>& ThisUtility, ModelPart& rBackground)
{
    std::string ConditionName("ChimeraThermalCouplingCondition2D");
    const Condition& rReferenceCondition = KratosComponents<Condition>::Get(ConditionName);
    ThisUtility.CreateBoundaryFaces(rBackground,rReferenceCondition);
}

namespace Python
{


  void  AddCustomUtilitiesToPython()
  {
    
    
    using namespace boost::python;

    typedef UblasSpace<double, CompressedMatrix, Vector> SparseSpaceType;
    typedef UblasSpace<double, Matrix, Vector> LocalSpaceType;
    typedef LinearSolver<SparseSpaceType, LocalSpaceType > LinearSolverType;


          /// Processes
      /*class_<ApplyMultipointConstraintsProcessChimera, boost::noncopyable, bases<Process>>("ApplyMultipointConstraintsProcessChimera", init<ModelPart&>())
      .def(init< ModelPart&, Parameters& >())
      .def("AddMasterSlaveRelation", &ApplyMultipointConstraintsProcessChimera::AddMasterSlaveRelation)
      .def("PrintData", &ApplyMultipointConstraintsProcessChimera::PrintData);*/

      class_<VtkOutput, boost::noncopyable>("VtkOutput", init< ModelPart&, std::string, Parameters >())
      .def("PrintOutput", &VtkOutput::PrintOutput)
      .def("PrintOutput", &VtkOutput::PrintOutputSubModelPart);      

      class_ < InterpolationUtility<2>, boost::noncopyable >( "InterpolationUtility2D", init<>() )
              .def("Test", &InterpolationUtility<2>::Test )
              //.def("DirichletBoundaryCalculation", &InterpolationUtility < 2 > ::DirichletBoundaryCalculation<double>)
              .def("Projection_Flux", &InterpolationUtility < 2 > ::Projection_Flux<double>)
              //.def("DirichletBoundaryCalculationKreis", &InterpolationUtility < 2 > ::DirichletBoundaryCalculationKreis<double>)
              //.def("NeumannBoundaryCalculationRechteck", &InterpolationUtility < 2 > ::NeumannBoundaryCalculationRechteck<double>)
              //.def("DirichletBoundaryCalculationRechteck", &InterpolationUtility < 2 > ::DirichletBoundaryCalculationRechteck<double>)
              .def("CreateFluidBoundaryFaces", AuxCreateFluidBoundaryFaces<2>)
              .def("CreateThermalBoundaryFaces", AuxCreateThermalBoundaryFaces<2>)
              //.def("DirichletBoundaryCalculationFlag", &InterpolationUtility < 2 > ::DirichletBoundaryCalculationFlag<double>)
              .def("Projection_Vector", &InterpolationUtility < 2 > ::Projection_Vector< array_1d<double,3> >)
              .def("Projection_Scalar", &InterpolationUtility < 2 > ::Projection_Vector< double >)
              .def("Projection_Traction", &InterpolationUtility < 2 > ::Projection_Traction)
              .def("Projection_Traction_Cavity", &InterpolationUtility < 2 > ::Projection_Traction_Cavity)
              .def("Projection_Traction_Nodal", &InterpolationUtility < 2 > ::Projection_Traction_Nodal)
              .def("Projection_Flux_Gauss", &InterpolationUtility < 2 > :: Projection_Flux_Gauss)
              ;

      class_ < InterpolationUtility<3>, boost::noncopyable >( "InterpolationUtility3D", init<>() )
              .def("Test", &InterpolationUtility<3>::Test )
              //.def("DirichletBoundaryCalculation", &InterpolationUtility < 2 > ::DirichletBoundaryCalculation<double>)

              //.def("DirichletBoundaryCalculationKreis", &InterpolationUtility < 2 > ::DirichletBoundaryCalculationKreis<double>)
              //.def("NeumannBoundaryCalculationRechteck", &InterpolationUtility < 2 > ::NeumannBoundaryCalculationRechteck<double>)
              //.def("DirichletBoundaryCalculationRechteck", &InterpolationUtility < 2 > ::DirichletBoundaryCalculationRechteck<double>)
              //.def("CreateBoundaryFaces", &InterpolationUtility < 3 > ::CreateBoundaryFaces)
              //.def("DirichletBoundaryCalculationFlag", &InterpolationUtility < 2 > ::DirichletBoundaryCalculationFlag<double>)
              .def("Projection_Vector", &InterpolationUtility < 3 > ::Projection_Vector< array_1d<double,3> >)
              .def("Projection_Scalar", &InterpolationUtility < 3 > ::Projection_Vector< double >)
              .def("Projection_Traction", &InterpolationUtility < 3 > ::Projection_Traction)
              .def("ExtractBoundaryFaces",&InterpolationUtility < 3 >::ExtractBoundaryFaces)
               ;
             
  // Periodic boundary conditions utilities
    typedef void (PeriodicConditionUtilitiesForChimera::*AddDoubleVariableType)(Properties&,Variable<double>&);
    typedef void (PeriodicConditionUtilitiesForChimera::*AddVariableComponentType)(Properties&,VariableComponent< VectorComponentAdaptor< array_1d<double, 3> > >&);

    AddDoubleVariableType AddDoubleVariable = &PeriodicConditionUtilitiesForChimera::AddPeriodicVariable;
    AddVariableComponentType AddVariableComponent = &PeriodicConditionUtilitiesForChimera::AddPeriodicVariable;

    class_<PeriodicConditionUtilitiesForChimera>("PeriodicConditionUtilitiesForChimera", init<ModelPart&,unsigned int>())
        .def("SetUpSearchStructure",&PeriodicConditionUtilitiesForChimera::SetUpSearchStructure)
        .def("DefinePeriodicBoundary",&PeriodicConditionUtilitiesForChimera::DefinePeriodicBoundary)
        .def("AddPeriodicVariable",AddDoubleVariable)
        .def("AddPeriodicVariable",AddVariableComponent)
    ;  


 // Base settings 
    typedef SolverSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType> BaseSettingsType;

    class_ < BaseSettingsType, boost::noncopyable >( "BaseSettingsType",no_init );

    // Fractional step settings
    enum_<FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::StrategyLabel>("StrategyLabel")
        .value("Velocity",FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::Velocity)
        .value("Pressure",FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::Pressure)
        //.value("EddyViscosity",FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::EddyViscosity)
    ;

    /* enum_<FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::TurbulenceModelLabel>("TurbulenceModelLabel")
        .value("SpalartAllmaras",FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::SpalartAllmaras)
    ;
 */
    typedef void (FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::*SetStrategyByParamsType)(FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::StrategyLabel const&,LinearSolverType::Pointer,const double,const unsigned int);
    //typedef void (FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::*BuildTurbModelType)(FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::TurbulenceModelLabel const&, LinearSolverType::Pointer, const double, const unsigned int);
    //typedef void (FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::*PassTurbModelType)(Process::Pointer);
    SetStrategyByParamsType ThisSetStrategyOverload = &FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::SetStrategy;
    //BuildTurbModelType SetTurbModel_Build = &FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::SetTurbulenceModel;
    //PassTurbModelType SetTurbModel_Pass = &FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::SetTurbulenceModel;

    class_< FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>,bases<BaseSettingsType>, boost::noncopyable>
        ("FractionalStepSettingsForChimera",init<ModelPart&,unsigned int,unsigned int,bool,bool,bool>())
        .def("SetStrategy",ThisSetStrategyOverload)
        //.def("SetTurbulenceModel",SetTurbModel_Build)
        //.def("SetTurbulenceModel",SetTurbModel_Pass)
        .def("GetStrategy",&FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::pGetStrategy)
        .def("SetEchoLevel",&FractionalStepSettingsForChimera<SparseSpaceType,LocalSpaceType,LinearSolverType>::SetEchoLevel)
    ; 
  }





}  // namespace Python.

} // Namespace Kratos
