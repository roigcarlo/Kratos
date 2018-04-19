
# Importing the Kratos Library
from KratosMultiphysics import *
from KratosMultiphysics.ConstitutiveModelsApplication import *
#from beam_sections_python_utility import SetProperties

def AssignMaterial(Properties):
# GUI property identifier: Property
    prop_id = 1;
    prop = Properties[prop_id]
    young_modulus = prop.GetValue(YOUNG_MODULUS)
    poisson_ratio = prop.GetValue(POISSON_RATIO)
    c10 = young_modulus*0.25/(1.0+poisson_ratio)
    print(c10)
    prop.SetValue( C10, c10)
    mat = LargeStrainPlaneStrain2DLaw(NeoHookeanModel())
    prop.SetValue(CONSTITUTIVE_LAW, mat.Clone())
