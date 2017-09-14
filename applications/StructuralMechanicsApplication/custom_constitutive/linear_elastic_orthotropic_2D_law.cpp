// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:		 BSD License
//					 license: structural_mechanics_application/license.txt
//
//  Main authors:    Peter Wilson
//       Contact:    A.Winterstein@tum.de
//

// System includes
#include <iostream>

// External includes
#include<cmath>

// Project includes
#include "includes/properties.h"
#include "custom_constitutive/linear_elastic_orthotropic_2D_law.hpp"

#include "structural_mechanics_application_variables.h"

namespace Kratos
{
	//******************************CONSTRUCTOR*********************************
	//**************************************************************************

	LinearElasticOrthotropic2DLaw::LinearElasticOrthotropic2DLaw()
		: ConstitutiveLaw()
	{
	}

	//******************************COPY CONSTRUCTOR****************************
	//**************************************************************************

	LinearElasticOrthotropic2DLaw::LinearElasticOrthotropic2DLaw
		(const LinearElasticOrthotropic2DLaw& rOther)
		: ConstitutiveLaw(rOther)
	{
	}

	//********************************CLONE*************************************
	//**************************************************************************

	ConstitutiveLaw::Pointer LinearElasticOrthotropic2DLaw::Clone() const
	{
		LinearElasticOrthotropic2DLaw::Pointer p_clone
			(new LinearElasticOrthotropic2DLaw(*this));
		return p_clone;
	}

	//*******************************DESTRUCTOR*********************************
	//**************************************************************************

	LinearElasticOrthotropic2DLaw::~LinearElasticOrthotropic2DLaw()
	{
	}
	
	//*****************************MATERIAL RESPONSES***************************
	//**************************************************************************

	void  LinearElasticOrthotropic2DLaw::CalculateMaterialResponsePK2(Parameters& rValues)
	{
		//1.- Lame constants
		//const double& YoungModulus = MaterialProperties[YOUNG_MODULUS];
		//const double& PoissonCoefficient = MaterialProperties[POISSON_RATIO];

		//a.-Check if the constitutive parameters are passed correctly to the law calculation
		//CheckParameters(rValues);

		//b.- Get Values to compute the constitutive law:
		Flags &Options = rValues.GetOptions();

		const Properties& MaterialProperties = rValues.GetMaterialProperties();

		Vector& StrainVector = rValues.GetStrainVector();
		Vector& StressVector = rValues.GetStressVector();

		//-----------------------------//

		if (Options.Is(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN))
		{
			//only needed
			const Matrix& DeformationGradientF = rValues.GetDeformationGradientF();

			//4.-Right Cauchy Green
			Matrix RightCauchyGreen = prod(trans(DeformationGradientF), DeformationGradientF);

			//5.-Green-Lagrange Strain:

			//E= 0.5*(FT*F-1)
			this->CalculateGreenLagrangeStrain(RightCauchyGreen, StrainVector);
		}

		//7.-Calculate Total PK2 stress

		if (Options.Is(ConstitutiveLaw::COMPUTE_STRESS))
		{
			if (Options.Is(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR))
			{
				Matrix& ConstitutiveMatrix = rValues.GetConstitutiveMatrix();
				this->CalculateLinearElasticMatrix(ConstitutiveMatrix, MaterialProperties);
				this->CalculateStress(StrainVector, ConstitutiveMatrix, StressVector);
			}
			else {
				Matrix ConstitutiveMatrix(StrainVector.size(), StrainVector.size());
				noalias(ConstitutiveMatrix) = ZeroMatrix(StrainVector.size(), StrainVector.size());

				this->CalculateLinearElasticMatrix(ConstitutiveMatrix, MaterialProperties);
				this->CalculateStress(StrainVector, ConstitutiveMatrix, StressVector);
			}
		}
		else if (Options.IsNot(ConstitutiveLaw::COMPUTE_STRESS) && Options.Is(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR))
		{
			Matrix& ConstitutiveMatrix = rValues.GetConstitutiveMatrix();
			this->CalculateLinearElasticMatrix(ConstitutiveMatrix, MaterialProperties);
		}
	}
	
	//***********************COMPUTE TOTAL STRAIN*****************************************
	//************************************************************************************

	void LinearElasticOrthotropic2DLaw::CalculateGreenLagrangeStrain(const Matrix & rRightCauchyGreen,
		Vector& rStrainVector)
	{
		// TAKEN FROM LinearElasticPlasticPlaneStrain2DLaw
		//E= 0.5*(FT*F-1)
		rStrainVector[0] = 0.5 * (rRightCauchyGreen(0, 0) - 1.00);
		rStrainVector[1] = 0.5 * (rRightCauchyGreen(1, 1) - 1.00);
		rStrainVector[2] = rRightCauchyGreen(0, 1);
	}

	//***********************COMPUTE TOTAL STRESS PK2*************************************
	//************************************************************************************

	void LinearElasticOrthotropic2DLaw::CalculateStress(const Vector & rStrainVector,
		const Matrix & rConstitutiveMatrix,
		Vector& rStressVector)
	{
		//1.-2nd Piola Kirchhoff StressVector increment
		if (rStressVector.size() != rStrainVector.size())
			rStressVector.resize(rStrainVector.size(), false);

		noalias(rStressVector) = prod(rConstitutiveMatrix, rStrainVector);
	}

	//***********************COMPUTE LINEAR ELASTIC MATRIX**********************
	//**************************************************************************

	void LinearElasticOrthotropic2DLaw::CalculateLinearElasticMatrix(Matrix& rConstitutiveMatrix,
		const Properties& rMaterialProperties)
	{
		//double G13 = G12;	// currently handled through "shell_cross_section.cpp"
		//double G23 = G12;	// currently handled through "shell_cross_section.cpp"

		double v21 = rMaterialProperties[POISSON_RATIO_XY]*rMaterialProperties[YOUNG_MODULUS_Y] / rMaterialProperties[YOUNG_MODULUS_X];

		double Q11 = rMaterialProperties[YOUNG_MODULUS_X] / (1.0 - rMaterialProperties[POISSON_RATIO_XY]*v21);
		double Q12 = rMaterialProperties[POISSON_RATIO_XY]*rMaterialProperties[YOUNG_MODULUS_Y] / (1.0 - rMaterialProperties[POISSON_RATIO_XY]*v21);
		double Q22 = rMaterialProperties[YOUNG_MODULUS_Y] / (1.0 - rMaterialProperties[POISSON_RATIO_XY]*v21);
		double Q66 = rMaterialProperties[SHEAR_MODULUS_XY];
		//double Q44 = G23;
		//double Q55 = G13;

		double theta = 0.0;	// rotation currently handled through 
		// "shell_cross_section.cpp" variable iPlyAngle. Left in for clarity.

		double c = cos(theta);
		double c2 = c*c;
		double c4 = c2 * c2;
		double s = sin(theta);
		double s2 = s*s;
		double s4 = s2*s2;

		rConstitutiveMatrix.clear();

		rConstitutiveMatrix(0, 0) = Q11*c4 + 2.0*(Q12 + 2.0*Q66)*s2*c2 + Q22*s4;				// Q11_hat
		rConstitutiveMatrix(0, 1) = (Q11 + Q22 - 4.0*Q66)*s2*c2 + Q12*(s4 + c4);				// Q12_hat
		rConstitutiveMatrix(0, 2) = (Q11 - Q12 - 2.0*Q66)*s*c2*c + (Q12 - Q22 + 2.0*Q66)*s*s2*c;// Q16_hat

		rConstitutiveMatrix(1, 0) = rConstitutiveMatrix(0, 1);
		rConstitutiveMatrix(1, 1) = Q11*s4 + 2.0 * (Q12 + 2.0*Q66)*s2*c2 + Q22*c4;				// Q22_hat
		rConstitutiveMatrix(1, 2) = (Q11 - Q12 - 2.0*Q66)*s2*s*c + (Q12 - Q22 + 2.0*Q66)*c2*c*s;// Q16_hat

		rConstitutiveMatrix(2, 0) = rConstitutiveMatrix(0, 2);
		rConstitutiveMatrix(2, 1) = rConstitutiveMatrix(1, 2);
		rConstitutiveMatrix(2, 2) = (Q11 + Q22 - 2.0*Q12 - 2.0*Q66)*s2*c2 + Q66*(s4 + c4);		//Q66_hat
	}

	//*************************CONSTITUTIVE LAW GENERAL FEATURES *************************
	//************************************************************************************

	
	void LinearElasticOrthotropic2DLaw::GetLawFeatures(Features& rFeatures)
	{
		//Set the type of law
		rFeatures.mOptions.Set(PLANE_STRESS_LAW);
		rFeatures.mOptions.Set(INFINITESIMAL_STRAINS);
		rFeatures.mOptions.Set(ANISOTROPIC);
		rFeatures.mOptions.Set(STENBERG_STABILIZATION_SUITABLE);

		//Set strain measure required by the consitutive law
		rFeatures.mStrainMeasures.push_back(StrainMeasure_Infinitesimal);
		rFeatures.mStrainMeasures.push_back(StrainMeasure_Deformation_Gradient);

		//Set the strain size
		rFeatures.mStrainSize = GetStrainSize();

		//Set the spacedimension
		rFeatures.mSpaceDimension = WorkingSpaceDimension();
	}
	

	//******************CHECK CONSISTENCY IN THE CONSTITUTIVE LAW*************************
	//************************************************************************************

	bool LinearElasticOrthotropic2DLaw::CheckParameters(Parameters& rValues)
	{
		return rValues.CheckAllParameters();
	}

	int LinearElasticOrthotropic2DLaw::Check(const Properties& rMaterialProperties,
		const GeometryType& rElementGeometry,
		const ProcessInfo& rCurrentProcessInfo)
	{
		if (YOUNG_MODULUS_X.Key() == 0 || !rMaterialProperties.Has(YOUNG_MODULUS_X))
			KRATOS_THROW_ERROR(std::invalid_argument, "YOUNG_MODULUS_X has Key zero or invalid value ", "")

			if (YOUNG_MODULUS_Y.Key() == 0 || !rMaterialProperties.Has(YOUNG_MODULUS_Y))
				KRATOS_THROW_ERROR(std::invalid_argument, "YOUNG_MODULUS_Y has Key zero or invalid value ", "")

				if (POISSON_RATIO_XY.Key() == 0 || !rMaterialProperties.Has(POISSON_RATIO_XY))
					KRATOS_THROW_ERROR(std::invalid_argument, "POISSON_RATIO_XY has Key zero invalid value ", "")

					if (DENSITY.Key() == 0 || !rMaterialProperties.Has(DENSITY))
						KRATOS_THROW_ERROR(std::invalid_argument, "DENSITY has Key zero or invalid value ", "")

						return 0;
	}
} // Namespace Kratos