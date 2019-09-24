//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Suneth Warnakulasuriya (https://github.com/sunethwarna)
//

#if !defined(KRATOS_EVM_MONOLITHIC_K_EPSILON_VMS_ADJOINT_ELEMENT_H_INCLUDED)
#define KRATOS_EVM_MONOLITHIC_K_EPSILON_VMS_ADJOINT_ELEMENT_H_INCLUDED

// System includes
#include <iterator>
#include <algorithm>

// External includes

// Project includes
#include "custom_elements/rans_evm_vms_adjoint_element.h"
#include "includes/checks.h"
#include "includes/element.h"
#include "includes/properties.h"

// Application includes
#include "custom_utilities/rans_calculation_utilities.h"
#include "rans_modelling_application_variables.h"
#include "utilities/adjoint_extensions.h"

#include "custom_elements/evm_k_epsilon/evm_epsilon_adjoint_element.h"
#include "custom_elements/evm_k_epsilon/evm_k_adjoint_element.h"
#include "custom_elements/evm_k_epsilon/evm_k_epsilon_vms_adjoint_element.h"

namespace Kratos
{
///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

template <unsigned int TDim, unsigned int TNumNodes = TDim + 1>
class EvmMonolithicKEpsilonVMSAdjointElement : public Element
{
    class ThisExtensions : public AdjointExtensions
    {
        Element* mpElement;

    public:
        explicit ThisExtensions(Element* pElement) : mpElement{pElement}
        {
        }

        void GetFirstDerivativesVector(std::size_t NodeId,
                                       std::vector<IndirectScalar<double>>& rVector,
                                       std::size_t Step) override
        {
            auto& r_node = mpElement->GetGeometry()[NodeId];
            rVector.resize(TDim + 3);
            std::size_t index = 0;
            rVector[index++] = MakeIndirectScalar(r_node, ADJOINT_FLUID_VECTOR_2_X, Step);
            rVector[index++] = MakeIndirectScalar(r_node, ADJOINT_FLUID_VECTOR_2_Y, Step);
            if (TDim == 3)
            {
                rVector[index++] =
                    MakeIndirectScalar(r_node, ADJOINT_FLUID_VECTOR_2_Z, Step);
            }
            rVector[index++] = IndirectScalar<double>{}; // pressure
            rVector[index++] = MakeIndirectScalar(r_node, RANS_SCALAR_1_ADJOINT_2, Step);
            rVector[index] = MakeIndirectScalar(r_node, RANS_SCALAR_2_ADJOINT_2, Step);
        }

        void GetSecondDerivativesVector(std::size_t NodeId,
                                        std::vector<IndirectScalar<double>>& rVector,
                                        std::size_t Step) override
        {
            auto& r_node = mpElement->GetGeometry()[NodeId];
            rVector.resize(TDim + 3);
            std::size_t index = 0;
            rVector[index++] = MakeIndirectScalar(r_node, ADJOINT_FLUID_VECTOR_3_X, Step);
            rVector[index++] = MakeIndirectScalar(r_node, ADJOINT_FLUID_VECTOR_3_Y, Step);
            if (TDim == 3)
            {
                rVector[index++] =
                    MakeIndirectScalar(r_node, ADJOINT_FLUID_VECTOR_3_Z, Step);
            }
            rVector[index++] = IndirectScalar<double>{}; // pressure
            rVector[index++] = MakeIndirectScalar(r_node, RANS_SCALAR_1_ADJOINT_3, Step);
            rVector[index] = MakeIndirectScalar(r_node, RANS_SCALAR_2_ADJOINT_3, Step);
        }

        void GetAuxiliaryVector(std::size_t NodeId,
                                std::vector<IndirectScalar<double>>& rVector,
                                std::size_t Step) override
        {
            auto& r_node = mpElement->GetGeometry()[NodeId];
            rVector.resize(TDim + 3);
            std::size_t index = 0;
            rVector[index++] =
                MakeIndirectScalar(r_node, AUX_ADJOINT_FLUID_VECTOR_1_X, Step);
            rVector[index++] =
                MakeIndirectScalar(r_node, AUX_ADJOINT_FLUID_VECTOR_1_Y, Step);
            if (TDim == 3)
            {
                rVector[index++] =
                    MakeIndirectScalar(r_node, AUX_ADJOINT_FLUID_VECTOR_1_Z, Step);
            }
            rVector[index++] = IndirectScalar<double>{}; // pressure
            rVector[index++] = MakeIndirectScalar(r_node, RANS_AUX_ADJOINT_SCALAR_1, Step);
            rVector[index] = MakeIndirectScalar(r_node, RANS_AUX_ADJOINT_SCALAR_2, Step);
        }

        void GetFirstDerivativesVariables(std::vector<VariableData const*>& rVariables) const override
        {
            rVariables.resize(3);
            rVariables[0] = &ADJOINT_FLUID_VECTOR_2;
            rVariables[1] = &RANS_SCALAR_1_ADJOINT_2;
            rVariables[2] = &RANS_SCALAR_2_ADJOINT_2;
        }

        void GetSecondDerivativesVariables(std::vector<VariableData const*>& rVariables) const override
        {
            rVariables.resize(3);
            rVariables[0] = &ADJOINT_FLUID_VECTOR_3;
            rVariables[1] = &RANS_SCALAR_1_ADJOINT_3;
            rVariables[2] = &RANS_SCALAR_2_ADJOINT_3;
        }

        void GetAuxiliaryVariables(std::vector<VariableData const*>& rVariables) const override
        {
            rVariables.resize(3);
            rVariables[0] = &AUX_ADJOINT_FLUID_VECTOR_1;
            rVariables[1] = &RANS_AUX_ADJOINT_SCALAR_1;
            rVariables[2] = &RANS_AUX_ADJOINT_SCALAR_2;
        }
    };

public:
    ///@name Type Definitions
    ///@{

    // defining the base type
    typedef Element BaseType;
    // defining the base adjoint base fluid element type
    typedef EvmKEpsilonVMSAdjointElement<TDim, TDim + 1, TDim + 3> AdjointFluidElement;
    // defining the k element type
    typedef EvmKAdjointElement<TDim, TNumNodes, TDim + 3, TDim + 1> AdjointKElement;
    // defining the epsilon element type
    typedef EvmEpsilonAdjointElement<TDim, TNumNodes, TDim + 3, TDim + 2> AdjointEpsilonElement;

    constexpr static unsigned int TFluidBlockSize = (TDim + 1);

    constexpr static unsigned int TFluidLocalSize = TFluidBlockSize * TNumNodes;

    constexpr static unsigned int TKBlockSize = 1;

    constexpr static unsigned int TKLocalSize = TKBlockSize * TNumNodes;

    constexpr static unsigned int TEpsilonBlockSize = 1;

    constexpr static unsigned int TEpsilonLocalSize = TEpsilonBlockSize * TNumNodes;

    constexpr static unsigned int TElementBlockSize =
        (TFluidBlockSize + TKBlockSize + TEpsilonBlockSize);

    constexpr static unsigned int TElementLocalSize = TElementBlockSize * TNumNodes;

    constexpr static unsigned int TCoordLocalSize = TDim * TNumNodes;

    // variable definitions
    typedef std::size_t IndexType;

    typedef Element::NodeType NodeType;

    typedef Element::NodesArrayType NodesArrayType;

    typedef Element::GeometryType GeometryType;

    typedef Element::PropertiesType PropertiesType;

    typedef Element::VectorType VectorType;

    typedef Element::MatrixType MatrixType;

    ///@name Pointer Definitions
    /// Pointer definition of EvmMonolithicKEpsilonVMSAdjointElement
    KRATOS_CLASS_POINTER_DEFINITION(EvmMonolithicKEpsilonVMSAdjointElement);

    ///@}

    ///@}
    ///@name Life Cycle
    ///@{

    /**
     * Constructor.
     */
    EvmMonolithicKEpsilonVMSAdjointElement(IndexType NewId = 0)
        : BaseType(NewId)
    {
    }

    /**
     * Constructor using Geometry
     */
    EvmMonolithicKEpsilonVMSAdjointElement(IndexType NewId, GeometryType::Pointer pGeometry)
        : BaseType(NewId, pGeometry)
    {
    }

    /**
     * Constructor using Properties
     */
    EvmMonolithicKEpsilonVMSAdjointElement(IndexType NewId,
                                           GeometryType::Pointer pGeometry,
                                           PropertiesType::Pointer pProperties)
        : BaseType(NewId, pGeometry, pProperties)
    {
    }

    /**
     * Destructor
     */
    ~EvmMonolithicKEpsilonVMSAdjointElement() override
    {
    }

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    void Initialize() override
    {
        this->SetValue(ADJOINT_EXTENSIONS, Kratos::make_shared<ThisExtensions>(this));
    }

    /**
     * ELEMENTS inherited from this class have to implement next
     * Create and Clone methods: MANDATORY
     */

    /**
     * creates a new element pointer
     * @param NewId: the ID of the new element
     * @param ThisNodes: the nodes of the new element
     * @param pProperties: the properties assigned to the new element
     * @return a Pointer to the new element
     */
    Element::Pointer Create(IndexType NewId,
                            NodesArrayType const& ThisNodes,
                            PropertiesType::Pointer pProperties) const override
    {
        KRATOS_TRY
        return Kratos::make_intrusive<EvmMonolithicKEpsilonVMSAdjointElement>(
            NewId, Element::GetGeometry().Create(ThisNodes), pProperties);
        KRATOS_CATCH("");
    }

    /**
     * creates a new element pointer
     * @param NewId: the ID of the new element
     * @param pGeom: the geometry to be employed
     * @param pProperties: the properties assigned to the new element
     * @return a Pointer to the new element
     */
    Element::Pointer Create(IndexType NewId,
                            GeometryType::Pointer pGeom,
                            PropertiesType::Pointer pProperties) const override
    {
        KRATOS_TRY
        return Kratos::make_intrusive<EvmMonolithicKEpsilonVMSAdjointElement>(
            NewId, pGeom, pProperties);
        KRATOS_CATCH("");
    }

    /**
     * creates a new element pointer and clones the previous element data
     * @param NewId: the ID of the new element
     * @param ThisNodes: the nodes of the new element
     * @param pProperties: the properties assigned to the new element
     * @return a Pointer to the new element
     */
    Element::Pointer Clone(IndexType NewId, NodesArrayType const& ThisNodes) const override
    {
        KRATOS_TRY
        return Kratos::make_intrusive<EvmMonolithicKEpsilonVMSAdjointElement>(
            NewId, Element::GetGeometry().Create(ThisNodes), Element::pGetProperties());
        KRATOS_CATCH("");
    }

    int Check(const ProcessInfo& rCurrentProcessInfo) override
    {
        KRATOS_TRY

        BaseType::Check(rCurrentProcessInfo);

        AdjointFluidElement adjoint_fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement adjoint_k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement adjoint_epsilon_element(this->Id(), this->pGetGeometry());

        adjoint_fluid_element.Check(rCurrentProcessInfo);
        adjoint_k_element.Check(rCurrentProcessInfo);
        adjoint_epsilon_element.Check(rCurrentProcessInfo);

        KRATOS_CHECK_VARIABLE_KEY(ADJOINT_FLUID_VECTOR_1);
        KRATOS_CHECK_VARIABLE_KEY(ADJOINT_FLUID_VECTOR_2);
        KRATOS_CHECK_VARIABLE_KEY(ADJOINT_FLUID_VECTOR_3);
        KRATOS_CHECK_VARIABLE_KEY(ADJOINT_FLUID_SCALAR_1);
        KRATOS_CHECK_VARIABLE_KEY(AUX_ADJOINT_FLUID_VECTOR_1);
        KRATOS_CHECK_VARIABLE_KEY(RANS_SCALAR_1_ADJOINT_1);
        KRATOS_CHECK_VARIABLE_KEY(RANS_SCALAR_1_ADJOINT_2);
        KRATOS_CHECK_VARIABLE_KEY(RANS_SCALAR_1_ADJOINT_3);
        KRATOS_CHECK_VARIABLE_KEY(RANS_AUX_ADJOINT_SCALAR_1);
        KRATOS_CHECK_VARIABLE_KEY(RANS_SCALAR_2_ADJOINT_1);
        KRATOS_CHECK_VARIABLE_KEY(RANS_SCALAR_2_ADJOINT_2);
        KRATOS_CHECK_VARIABLE_KEY(RANS_SCALAR_2_ADJOINT_3);
        KRATOS_CHECK_VARIABLE_KEY(RANS_AUX_ADJOINT_SCALAR_2);

        for (IndexType i_node = 0; i_node < TNumNodes; ++i_node)
        {
            NodeType& r_node = this->GetGeometry()[i_node];
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ADJOINT_FLUID_VECTOR_1, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ADJOINT_FLUID_VECTOR_2, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ADJOINT_FLUID_VECTOR_3, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ADJOINT_FLUID_SCALAR_1, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(AUX_ADJOINT_FLUID_VECTOR_1, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_SCALAR_1_ADJOINT_1, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_SCALAR_1_ADJOINT_2, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_SCALAR_1_ADJOINT_3, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_AUX_ADJOINT_SCALAR_1, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_SCALAR_2_ADJOINT_1, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_SCALAR_2_ADJOINT_2, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_SCALAR_2_ADJOINT_3, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(RANS_AUX_ADJOINT_SCALAR_2, r_node);
        }

        return 0;

        KRATOS_CATCH("");
    }

    /**
     * this determines the elemental equation ID vector for all elemental
     * DOFs
     * @param rResult the elemental equation ID vector
     * @param rCurrentProcessInfo the current process info instance
     */
    void EquationIdVector(EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo) override
    {
        if (rResult.size() != TElementLocalSize)
            rResult.resize(TElementLocalSize);

        AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

        fluid_element.SetData(this->Data());
        k_element.SetData(this->Data());
        epsilon_element.SetData(this->Data());

        std::array<std::size_t, TFluidLocalSize> fluid_ids;
        fluid_element.EquationIdArray(fluid_ids, rCurrentProcessInfo);
        AssignSubArray(fluid_ids, rResult, VelPresBlock());
        std::array<std::size_t, TKLocalSize> k_ids;
        k_element.EquationIdArray(k_ids, rCurrentProcessInfo);
        AssignSubArray(k_ids, rResult, KBlock());
        std::array<std::size_t, TEpsilonLocalSize> epsilon_ids;
        epsilon_element.EquationIdArray(epsilon_ids, rCurrentProcessInfo);
        AssignSubArray(epsilon_ids, rResult, EpsilonBlock());
    }

    /**
     * determines the elemental list of DOFs
     * @param ElementalDofList the list of DOFs
     * @param rCurrentProcessInfo the current process info instance
     */
    void GetDofList(DofsVectorType& rElementalDofList, ProcessInfo& rCurrentProcessInfo) override
    {
        if (rElementalDofList.size() != TElementLocalSize)
            rElementalDofList.resize(TElementLocalSize);

        AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

        fluid_element.SetData(this->Data());
        k_element.SetData(this->Data());
        epsilon_element.SetData(this->Data());

        std::array<Dof<double>::Pointer, TFluidLocalSize> fluid_dofs;
        fluid_element.GetDofArray(fluid_dofs, rCurrentProcessInfo);
        AssignSubArray(fluid_dofs, rElementalDofList, VelPresBlock());
        std::array<Dof<double>::Pointer, TKLocalSize> k_dofs;
        k_element.GetDofArray(k_dofs, rCurrentProcessInfo);
        AssignSubArray(k_dofs, rElementalDofList, KBlock());
        std::array<Dof<double>::Pointer, TEpsilonLocalSize> epsilon_dofs;
        epsilon_element.GetDofArray(epsilon_dofs, rCurrentProcessInfo);
        AssignSubArray(epsilon_dofs, rElementalDofList, EpsilonBlock());
    }

    /// Returns the adjoint values stored in this element's nodes.
    void GetValuesVector(VectorType& rValues, int Step = 0) override
    {
        if (rValues.size() != TElementLocalSize)
            rValues.resize(TElementLocalSize, false);

        AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

        fluid_element.SetData(this->Data());
        k_element.SetData(this->Data());
        epsilon_element.SetData(this->Data());

        std::array<double, TFluidLocalSize> fluid_values;
        fluid_element.GetValuesArray(fluid_values, Step);
        AssignSubArray(fluid_values, rValues, VelPresBlock());
        std::array<double, TKLocalSize> k_values;
        k_element.GetValuesArray(k_values, Step);
        AssignSubArray(k_values, rValues, KBlock());
        std::array<double, TEpsilonLocalSize> epsilon_values;
        epsilon_element.GetValuesArray(epsilon_values, Step);
        AssignSubArray(epsilon_values, rValues, EpsilonBlock());
    }

    /// Returns the adjoint velocity values stored in this element's nodes.
    void GetFirstDerivativesVector(VectorType& rValues, int Step = 0) override
    {
        if (rValues.size() != TElementLocalSize)
            rValues.resize(TElementLocalSize, false);

        AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

        fluid_element.SetData(this->Data());
        k_element.SetData(this->Data());
        epsilon_element.SetData(this->Data());

        std::array<double, TFluidLocalSize> fluid_values;
        fluid_element.GetFirstDerivativesArray(fluid_values, Step);
        AssignSubArray(fluid_values, rValues, VelPresBlock());
        std::array<double, TKLocalSize> k_values;
        k_element.GetFirstDerivativesArray(k_values, Step);
        AssignSubArray(k_values, rValues, KBlock());
        std::array<double, TEpsilonLocalSize> epsilon_values;
        epsilon_element.GetFirstDerivativesArray(epsilon_values, Step);
        AssignSubArray(epsilon_values, rValues, EpsilonBlock());
    }

    void GetSecondDerivativesVector(VectorType& rValues, int Step) override
    {
        if (rValues.size() != TElementLocalSize)
            rValues.resize(TElementLocalSize, false);

        AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

        fluid_element.SetData(this->Data());
        k_element.SetData(this->Data());
        epsilon_element.SetData(this->Data());

        std::array<double, TFluidLocalSize> fluid_values;
        fluid_element.GetSecondDerivativesArray(fluid_values, Step);
        AssignSubArray(fluid_values, rValues, VelPresBlock());
        std::array<double, TKLocalSize> k_values;
        k_element.GetSecondDerivativesArray(k_values, Step);
        AssignSubArray(k_values, rValues, KBlock());
        std::array<double, TEpsilonLocalSize> epsilon_values;
        epsilon_element.GetSecondDerivativesArray(epsilon_values, Step);
        AssignSubArray(epsilon_values, rValues, EpsilonBlock());
    }

    void CalculateLocalSystem(MatrixType& rLeftHandSideMatrix,
                              VectorType& rRightHandSideVector,
                              ProcessInfo& rCurrentProcessInfo) override
    {
        KRATOS_TRY

        KRATOS_ERROR << "EvmMonolithicKEpsilonVMSAdjointElement::"
                        "CalculateLocalSystem method is not implemented.";

        KRATOS_CATCH("");
    }

    void CalculateLeftHandSide(MatrixType& rLeftHandSideMatrix,
                               ProcessInfo& /*rCurrentProcessInfo*/) override
    {
        if (rLeftHandSideMatrix.size1() != TElementLocalSize ||
            rLeftHandSideMatrix.size2() != TElementLocalSize)
            rLeftHandSideMatrix.resize(TElementLocalSize, TElementLocalSize, false);

        rLeftHandSideMatrix.clear();
    }

    void CalculateRightHandSide(VectorType& rRightHandSideVector,
                                ProcessInfo& /*rCurrentProcessInfo*/) override
    {
        KRATOS_TRY

        KRATOS_ERROR << "EvmMonolithicKEpsilonVMSAdjointElement::"
                        "CalculateRightHandSide method is not implemented.";

        KRATOS_CATCH("");
    }

    void CalculateFirstDerivativesLHS(MatrixType& rLeftHandSideMatrix,
                                      ProcessInfo& rCurrentProcessInfo) override
    {
        KRATOS_TRY

        AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

        fluid_element.SetData(this->Data());
        k_element.SetData(this->Data());
        epsilon_element.SetData(this->Data());

        if (rLeftHandSideMatrix.size1() != TElementLocalSize ||
            rLeftHandSideMatrix.size2() != TElementLocalSize)
            rLeftHandSideMatrix.resize(TElementLocalSize, TElementLocalSize, false);

        rLeftHandSideMatrix.clear();

        BoundedMatrix<double, TFluidLocalSize, TFluidLocalSize> vms_vms;
        fluid_element.CalculateFirstDerivativesLHS(vms_vms, rCurrentProcessInfo);
        AssignSubMatrix(vms_vms, rLeftHandSideMatrix, VelPresBlock(), VelPresBlock());

        BoundedMatrix<double, TNumNodes, TFluidLocalSize> vms_k;
        fluid_element.CalculateResidualScalarDerivatives(
            TURBULENT_KINETIC_ENERGY, vms_k, rCurrentProcessInfo);
        AssignSubMatrix(vms_k, rLeftHandSideMatrix, KBlock(), VelPresBlock());

        BoundedMatrix<double, TNumNodes, TFluidLocalSize> vms_epsilon;
        fluid_element.CalculateResidualScalarDerivatives(
            TURBULENT_ENERGY_DISSIPATION_RATE, vms_epsilon, rCurrentProcessInfo);
        AssignSubMatrix(vms_epsilon, rLeftHandSideMatrix, EpsilonBlock(), VelPresBlock());

        BoundedMatrix<double, TNumNodes * TDim, TNumNodes> k_vms;
        k_element.CalculateElementTotalResidualVelocityDerivatives(k_vms, rCurrentProcessInfo);
        AssignSubMatrix(k_vms, rLeftHandSideMatrix, VelBlock(), KBlock());

        BoundedMatrix<double, TNumNodes, TNumNodes> k_k;
        k_element.CalculateFirstDerivativesLHS(k_k, rCurrentProcessInfo);
        AssignSubMatrix(k_k, rLeftHandSideMatrix, KBlock(), KBlock());

        BoundedMatrix<double, TNumNodes, TNumNodes> k_epsilon;
        k_element.CalculateResidualScalarDerivatives(
            TURBULENT_ENERGY_DISSIPATION_RATE, k_epsilon, rCurrentProcessInfo);
        AssignSubMatrix(k_epsilon, rLeftHandSideMatrix, EpsilonBlock(), KBlock());

        BoundedMatrix<double, TNumNodes * TDim, TNumNodes> epsilon_vms;
        epsilon_element.CalculateElementTotalResidualVelocityDerivatives(
            epsilon_vms, rCurrentProcessInfo);
        AssignSubMatrix(epsilon_vms, rLeftHandSideMatrix, VelBlock(), EpsilonBlock());

        BoundedMatrix<double, TNumNodes, TNumNodes> epsilon_k;
        epsilon_element.CalculateResidualScalarDerivatives(
            TURBULENT_KINETIC_ENERGY, epsilon_k, rCurrentProcessInfo);
        AssignSubMatrix(epsilon_k, rLeftHandSideMatrix, KBlock(), EpsilonBlock());

        BoundedMatrix<double, TNumNodes, TNumNodes> epsilon_epsilon;
        epsilon_element.CalculateFirstDerivativesLHS(epsilon_epsilon, rCurrentProcessInfo);
        AssignSubMatrix(epsilon_epsilon, rLeftHandSideMatrix, EpsilonBlock(),
                        EpsilonBlock());

        KRATOS_CATCH("");
    }

    void CalculateSecondDerivativesLHS(MatrixType& rLeftHandSideMatrix,
                                       ProcessInfo& rCurrentProcessInfo) override
    {
        KRATOS_TRY

        AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
        AdjointKElement k_element(this->Id(), this->pGetGeometry());
        AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

        fluid_element.SetData(this->Data());
        k_element.SetData(this->Data());
        epsilon_element.SetData(this->Data());

        if (rLeftHandSideMatrix.size1() != TElementLocalSize ||
            rLeftHandSideMatrix.size2() != TElementLocalSize)
            rLeftHandSideMatrix.resize(TElementLocalSize, TElementLocalSize, false);

        rLeftHandSideMatrix.clear();

        BoundedMatrix<double, TFluidLocalSize, TFluidLocalSize> vms_vms;
        fluid_element.CalculateSecondDerivativesLHS(vms_vms, rCurrentProcessInfo);
        AssignSubMatrix(vms_vms, rLeftHandSideMatrix, VelPresBlock(), VelPresBlock());

        BoundedMatrix<double, TNumNodes, TNumNodes> k_k;
        k_element.CalculateSecondDerivativesLHS(k_k, rCurrentProcessInfo);
        AssignSubMatrix(k_k, rLeftHandSideMatrix, KBlock(), KBlock());

        BoundedMatrix<double, TNumNodes, TNumNodes> epsilon_epsilon;
        epsilon_element.CalculateSecondDerivativesLHS(epsilon_epsilon, rCurrentProcessInfo);
        AssignSubMatrix(epsilon_epsilon, rLeftHandSideMatrix, EpsilonBlock(),
                        EpsilonBlock());

        KRATOS_CATCH("");
    }

    void CalculateMassMatrix(MatrixType& rMassMatrix, ProcessInfo& /*rCurrentProcessInfo*/) override
    {
        KRATOS_TRY

        KRATOS_ERROR << "EvmMonolithicKEpsilonVMSAdjointElement::"
                        "CalculateMassMatrix method is not implemented.";

        KRATOS_CATCH("")
    }

    void CalculateDampingMatrix(MatrixType& rDampingMatrix,
                                ProcessInfo& /*rCurrentProcessInfo*/) override
    {
        KRATOS_TRY

        KRATOS_ERROR << "EvmMonolithicKEpsilonVMSAdjointElement::"
                        "CalculateDampingMatrix method is not implemented.";

        KRATOS_CATCH("")
    }

    void CalculateSensitivityMatrix(const Variable<array_1d<double, 3>>& rSensitivityVariable,
                                    Matrix& rOutput,
                                    const ProcessInfo& rCurrentProcessInfo) override
    {
        KRATOS_TRY

        if (rSensitivityVariable == SHAPE_SENSITIVITY)
        {
            if (rOutput.size1() != TCoordLocalSize || rOutput.size2() != TElementLocalSize)
                rOutput.resize(TCoordLocalSize, TElementLocalSize, false);

            rOutput.clear();

            AdjointFluidElement fluid_element(this->Id(), this->pGetGeometry());
            AdjointKElement k_element(this->Id(), this->pGetGeometry());
            AdjointEpsilonElement epsilon_element(this->Id(), this->pGetGeometry());

            fluid_element.SetData(this->Data());
            k_element.SetData(this->Data());
            epsilon_element.SetData(this->Data());

            Matrix vms;
            fluid_element.CalculateSensitivityMatrix(rSensitivityVariable, vms,
                                                     rCurrentProcessInfo);
            AssignSubMatrix(vms, rOutput, CoordBlock(), VelPresBlock());
            k_element.CalculateSensitivityMatrix(rSensitivityVariable, rOutput,
                                                 rCurrentProcessInfo);
            epsilon_element.CalculateSensitivityMatrix(
                rSensitivityVariable, rOutput, rCurrentProcessInfo);
        }
        else
        {
            KRATOS_ERROR << "Sensitivity variable " << rSensitivityVariable
                         << " not supported." << std::endl;
        }

        KRATOS_CATCH("")
    }

    ///@}
    ///@name Access
    ///@{

    ///@}
    ///@name Inquiry
    ///@{

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
        std::stringstream buffer;
        buffer << "EvmMonolithicKEpsilonVMSAdjointElement #" << Element::Id();
        return buffer.str();
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "EvmMonolithicKEpsilonVMSAdjointElement #" << Element::Id();
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override
    {
        Element::pGetGeometry()->PrintData(rOStream);
    }

    ///@}
    ///@name Friends
    ///@{

    ///@}

protected:
    ///@name Protected Operations
    ///@{
    ///@}
private:
    ///@name Member Variables
    ///@{

    ///@}
    ///@name Unaccessible methods
    ///@{

    struct SubBlockLayout
    {
        std::size_t SubBlockOffset;
        std::size_t SubBlockSize;
        std::size_t BlockSize;
        std::size_t NumBlocks;
    };

    constexpr SubBlockLayout CoordBlock()
    {
        return {0, TDim, TDim, TNumNodes};
    }

    constexpr SubBlockLayout VelBlock()
    {
        return {0, TDim, TDim + 3, TNumNodes};
    }

    constexpr SubBlockLayout VelPresBlock()
    {
        return {0, TDim + 1, TDim + 3, TNumNodes};
    }

    constexpr SubBlockLayout KBlock()
    {
        return {TDim + 1, 1, TDim + 3, TNumNodes};
    }

    constexpr SubBlockLayout EpsilonBlock()
    {
        return {TDim + 2, 1, TDim + 3, TNumNodes};
    }

    constexpr std::size_t MonolithicIndex(SubBlockLayout L, std::size_t SubIndex)
    {
        return SubIndex + (SubIndex / L.SubBlockSize) * (L.BlockSize - L.SubBlockSize) +
               L.SubBlockOffset;
    }

    template <class TMatrix1, class TMatrix2>
    void AssignSubMatrix(const TMatrix1& rSrc, TMatrix2& rDest, SubBlockLayout RowLayout, SubBlockLayout ColLayout)
    {
        for (std::size_t i = 0; i < rSrc.size1(); ++i)
            for (std::size_t j = 0; j < rSrc.size2(); ++j)
                rDest(MonolithicIndex(RowLayout, i), MonolithicIndex(ColLayout, j)) =
                    rSrc(i, j);
    }

    template <class TArray1, class TArray2>
    void AssignSubArray(const TArray1& rSrc, TArray2& rDest, SubBlockLayout Layout)
    {
        for (std::size_t i = 0; i < rSrc.size(); ++i)
            rDest[MonolithicIndex(Layout, i)] = rSrc[i];
    }

    ///@}
};

///@}

} // namespace Kratos

#endif