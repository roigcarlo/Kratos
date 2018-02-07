#include "custom_io/hdf5_model_part_io.h"

#include "custom_utilities/hdf5_points_data.h"
#include "custom_utilities/hdf5_connectivities_data.h"
#include "custom_io/hdf5_nodal_solution_step_variables_io.h"
#include "custom_io/hdf5_data_value_container_io.h"
#include "custom_utilities/factor_elements_and_conditions_utility.h"

namespace Kratos
{
namespace HDF5
{
ModelPartIO::ModelPartIO(Parameters Settings, File::Pointer pFile)
: ModelPartIOBase(Settings, pFile)
{
    KRATOS_TRY;

    Check();

    KRATOS_CATCH("");
}

bool ModelPartIO::ReadNodes(NodesContainerType& rNodes)
{
    KRATOS_TRY;

    rNodes.clear();

    const std::size_t num_nodes = ReadNodesNumber();
    Internals::PointsData points;

    points.ReadData(*mpFile, mPrefix + "/Nodes/Local", 0, num_nodes);
    points.CreateNodes(rNodes);

    return true;
    KRATOS_CATCH("");
}

void ModelPartIO::WriteNodes(NodesContainerType const& rNodes)
{
    KRATOS_TRY;

    Internals::PointsData points;
    points.SetData(rNodes);
    WriteInfo info;
    points.WriteData(*mpFile, mPrefix + "/Nodes/Local", info);
    
    KRATOS_CATCH("");
}

void ModelPartIO::ReadElements(NodesContainerType& rNodes,
                               PropertiesContainerType& rProperties,
                               ElementsContainerType& rElements)
{
    KRATOS_TRY;

    rElements.clear();
    std::vector<std::string> group_names;
    mpFile->GetGroupNames(mPrefix + "/Elements", group_names);

    for (const auto& r_name : group_names)
    {
        int num_elems;
        mpFile->ReadAttribute(mPrefix + "/Elements/" + r_name, "Size", num_elems);
        Internals::ConnectivitiesData connectivities;
        connectivities.ReadData(*mpFile, mPrefix + "/Elements/" + r_name, 0, num_elems);
        connectivities.CreateEntities(rNodes, rProperties, rElements);
    }

    KRATOS_CATCH("");
}

void ModelPartIO::WriteElements(ElementsContainerType const& rElements)
{
    KRATOS_TRY;

    FactorElementsUtility factored_elements(rElements);

    WriteInfo info;
    for (const auto& r_elems : factored_elements)
    {
        Internals::ConnectivitiesData connectivities;
        connectivities.SetData(r_elems.second);
        connectivities.WriteData(*mpFile, mPrefix + "/Elements/" + r_elems.first, info);
        const int size = info.TotalSize;
        mpFile->WriteAttribute(mPrefix + "/Elements/" + r_elems.first, "Size", size);
    }

    KRATOS_CATCH("");
}

void ModelPartIO::ReadConditions(NodesContainerType& rNodes,
                                 PropertiesContainerType& rProperties,
                                 ConditionsContainerType& rConditions)
{
    KRATOS_TRY;

    rConditions.clear();
    std::vector<std::string> group_names;
    mpFile->GetGroupNames(mPrefix + "/Conditions", group_names);

    for (const auto& r_name : group_names)
    {
        int num_conds;
        mpFile->ReadAttribute(mPrefix + "/Conditions/" + r_name, "Size", num_conds);
        Internals::ConnectivitiesData connectivities;
        connectivities.ReadData(*mpFile, mPrefix + "/Conditions/" + r_name, 0, num_conds);
        connectivities.CreateEntities(rNodes, rProperties, rConditions);
    }

    KRATOS_CATCH("");
}

void ModelPartIO::WriteConditions(ConditionsContainerType const& rConditions)
{
    KRATOS_TRY;

    FactorConditionsUtility factored_conditions(rConditions);

    WriteInfo info;
    for (const auto& r_conds : factored_conditions)
    {
        Internals::ConnectivitiesData connectivities;
        connectivities.SetData(r_conds.second);
        connectivities.WriteData(*mpFile, mPrefix + "/Conditions/" + r_conds.first, info);
        const int size = info.TotalSize;
        mpFile->WriteAttribute(mPrefix + "/Conditions/" + r_conds.first, "Size", size);
    }

    KRATOS_CATCH("");
}

void ModelPartIO::ReadModelPart(ModelPart& rModelPart)
{
    KRATOS_TRY;

    ReadProperties(rModelPart.rProperties());
    Internals::DataValueContainerIO process_info_io(mPrefix + "/ProcessInfo", mpFile);
    process_info_io.ReadDataValueContainer(rModelPart.GetProcessInfo());
    ReadNodes(rModelPart.Nodes());
    ReadElements(rModelPart.Nodes(), rModelPart.rProperties(), rModelPart.Elements());
    ReadConditions(rModelPart.Nodes(), rModelPart.rProperties(), rModelPart.Conditions());
    Internals::NodalSolutionStepVariablesIO nodal_variables_io(mPrefix, mpFile);
    nodal_variables_io.ReadAndAssignVariablesList(rModelPart);
    nodal_variables_io.ReadAndAssignBufferSize(rModelPart);

    KRATOS_CATCH("");
}

void ModelPartIO::Check()
{
    KRATOS_TRY;

    KRATOS_ERROR_IF(mpFile->GetTotalProcesses() != 1)
        << "Serial IO expects file access by a single process only." << std::endl;

    KRATOS_CATCH("");
}

} // namespace HDF5.
} // namespace Kratos.
