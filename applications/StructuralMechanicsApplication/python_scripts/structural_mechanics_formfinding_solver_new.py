from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7

# Importing the Kratos Library
import KratosMultiphysics

# Import applications
import KratosMultiphysics.StructuralMechanicsApplication as StructuralMechanicsApplication

# Import base class file
from KratosMultiphysics.StructuralMechanicsApplication.structural_mechanics_solver import MechanicalSolver

def CreateSolver(main_model_part, custom_settings):
    return FormfindingMechanicalSolverNew(main_model_part, custom_settings)

class FormfindingMechanicalSolverNew(MechanicalSolver):
    """The structural mechanics formfinding solver.

    This class creates the mechanical solver for formfinding.

    See structural_mechanics_solver.py for more information.
    """
    def __init__(self, main_model_part, custom_settings):
        if not custom_settings.Has("use_computing_model_part"):
            custom_settings.AddEmptyValue("use_computing_model_part")
        custom_settings["use_computing_model_part"].SetBool(False) # must be False for FormFinding!

        # Construct the base solver.
        super(FormfindingMechanicalSolverNew, self).__init__(main_model_part, custom_settings)
        KratosMultiphysics.Logger.PrintInfo("::[FormfindingMechanicalSolverNew]:: ", "Construction finished")


    @classmethod
    def GetDefaultSettings(cls):
        this_defaults = KratosMultiphysics.Parameters("""{
            "printing_format"             : "all",
            "formfinding_model_part_name" : "",
            "projection_settings": {
                "model_part_name"  : "Structure",
                "echo_level"       : 0,
                "projection_type"  : "planar",
                "global_direction" : [1,0,0],
                "variable_name"    : "PLEASE_SPECIFY",
                "visualize_in_vtk" : false,
                "method_specific_settings" : { }
            }
        }""")
        this_defaults.AddMissingParameters(super(FormfindingMechanicalSolverNew, cls).GetDefaultSettings())
        return this_defaults

    def _create_solution_scheme(self):
        return KratosMultiphysics.ResidualBasedIncrementalUpdateStaticScheme()

    def _create_mechanical_solution_strategy(self):
        computing_model_part = self.GetComputingModelPart()
        mechanical_scheme = self.get_solution_scheme()
        linear_solver = self.get_linear_solver()
        mechanical_convergence_criterion = self.get_convergence_criterion()
        builder_and_solver = self.get_builder_and_solver()


        # in some cases not all elements need to be reset by the formfinding strategy
        formfinding_model_part = self.GetComputingModelPart()
        if len(self.settings["formfinding_model_part_name"].GetString())>0:
            formfinding_model_part = computing_model_part.GetSubModelPart(self.settings["formfinding_model_part_name"].GetString())

        possible_printing_choices = ["all","vtk","gid"]
        chosen_printing_format = self.settings["printing_format"].GetString()

        if chosen_printing_format not in possible_printing_choices:
            err_msg =  "The requested printing type \"" + chosen_printing_format + "\" is not available!\n"
            err_msg += "Available options are: "
            for i in range(len(possible_printing_choices)): err_msg += "\""+possible_printing_choices[i]+"\" "
            raise Exception(err_msg)


        return StructuralMechanicsApplication.FormfindingStrategy(
                                                                computing_model_part,
                                                                mechanical_scheme,
                                                                linear_solver,
                                                                mechanical_convergence_criterion,
                                                                builder_and_solver,
                                                                formfinding_model_part,
                                                                self.settings["printing_format"].GetString(),
                                                                self.settings["projection_settings"],
                                                                self.settings["max_iteration"].GetInt(),
                                                                self.settings["compute_reactions"].GetBool(),
                                                                self.settings["reform_dofs_at_each_step"].GetBool(),
                                                                self.settings["move_mesh_flag"].GetBool())

