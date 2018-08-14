#include "Variable/VariableController2.h"
#include "Variable/VariableSynchronizationGroup2.h"
#include <Common/containers.h>
#include <Common/debug.h>
#include <Data/DataProviderParameters.h>
#include <Data/DateTimeRangeHelper.h>
#include <Variable/VariableCacheStrategyFactory.h>

class VariableController2::VariableController2Private
{
    QMap<QUuid,std::shared_ptr<Variable>> _variables;
    QMap<QUuid,std::shared_ptr<IDataProvider>> _providers;
    QMap<QUuid,std::shared_ptr<VariableSynchronizationGroup2>> _synchronizationGroups;
    std::unique_ptr<VariableCacheStrategy> _cacheStrategy;
    bool p_contains(std::shared_ptr<Variable> variable)
    {
        return _providers.contains(variable->ID());
    }
    bool v_contains(std::shared_ptr<Variable> variable)
    {
        return SciQLop::containers::contains(this->_variables, variable);
    }
    bool sg_contains(std::shared_ptr<Variable> variable)
    {
        return _synchronizationGroups.contains(variable->ID());
    }

    void _changeRange(std::shared_ptr<Variable> var, DateTimeRange r)
    {
        auto provider = _providers[var->ID()];
        auto missingRanges = r - var->range();
        for(auto range:missingRanges)
        {
            auto data = provider->getData(DataProviderParameters{{range},var->metadata()});
            var->mergeDataSeries(data);
        }
        var->setRange(r);
    }
public:
    VariableController2Private(QObject* parent=Q_NULLPTR)
        :_cacheStrategy(VariableCacheStrategyFactory::createCacheStrategy(CacheStrategy::Proportional))
    {
        Q_UNUSED(parent);
    }

    ~VariableController2Private() = default;

    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider, const DateTimeRange &range)
    {
        auto newVar = std::make_shared<Variable>(name,metadata);
        this->_variables[newVar->ID()] = newVar;
        this->_providers[newVar->ID()] = provider;
        this->_synchronizationGroups[newVar->ID()] = std::make_shared<VariableSynchronizationGroup2>(newVar->ID());
        return newVar;
    }

    void deleteVariable(std::shared_ptr<Variable> variable)
    {
        /*
         * Removing twice a var is ok but a var without provider has to be a hard error
         * this means we got the var controller in an inconsistent state
         */
        if(v_contains(variable))
            this->_variables.remove(variable->ID());
        if(p_contains(variable))
            this->_providers.remove(variable->ID());
        else
            SCIQLOP_ERROR(VariableController2Private, "No provider found for given variable");
    }

    void changeRange(std::shared_ptr<Variable> variable, DateTimeRange r)
    {
        if(p_contains(variable))
        {
            if(!std::isnan(r.m_TStart) && !std::isnan(r.m_TEnd))
            {
                auto transformation = DateTimeRangeHelper::computeTransformation(variable->range(),r);
                auto group = _synchronizationGroups[variable->ID()];
                if(std::holds_alternative<DateTimeRangeTransformation>(transformation))
                {
                    for(auto varId:group->variables())
                    {
                        auto var = _variables[varId];
                        auto newRange = var->range().transform(std::get<DateTimeRangeTransformation>(transformation));
                        _changeRange(var,newRange);
                    }
                }
                else // force new range to all variables -> may be weird if more than one var in the group
                    // @TODO ensure that there is no side effects
                {
                    for(auto varId:group->variables())
                    {
                        auto var = _variables[varId];
                        _changeRange(var,r);
                    }
                }
            }
            else
            {
                SCIQLOP_ERROR(VariableController2Private, "Invalid range containing NaN");
            }
        }
        else
        {
            SCIQLOP_ERROR(VariableController2Private, "No provider found for given variable");
        }
    }

    void synchronize(std::shared_ptr<Variable> var, std::shared_ptr<Variable> with)
    {
        if(v_contains(var) && v_contains(with))
        {
            if(sg_contains(var) && sg_contains(with))
            {

                auto dest_group = this->_synchronizationGroups[with->ID()];
                this->_synchronizationGroups[var->ID()] = dest_group;
                dest_group->addVariable(var->ID());
            }
            else
            {
                SCIQLOP_ERROR(VariableController2Private, "At least one of the given variables isn't in a sync group");
            }
        }
        else
        {
            SCIQLOP_ERROR(VariableController2Private, "At least one of the given variables is not found");
        }
    }

    const std::set<std::shared_ptr<Variable>> variables()
    {
        std::set<std::shared_ptr<Variable>> vars;
        for(auto var:_variables.values())
        {
            vars.insert(var);
        }
        return  vars;
    }

};

VariableController2::VariableController2()
    :impl{spimpl::make_unique_impl<VariableController2Private>()}
{}

std::shared_ptr<Variable> VariableController2::createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider, const DateTimeRange &range)
{
    auto var =  impl->createVariable(name, metadata, provider, range);
    emit variableAdded(var);
    if(!std::isnan(range.m_TStart) && !std::isnan(range.m_TEnd))
        impl->changeRange(var,range);
    else
        SCIQLOP_ERROR(VariableController2, "Creating a variable with default constructed DateTimeRange is an error");
    return var;
}

void VariableController2::deleteVariable(std::shared_ptr<Variable> variable)
{
    impl->deleteVariable(variable);
    emit variableDeleted(variable);
}

void VariableController2::changeRange(std::shared_ptr<Variable> variable, DateTimeRange r)
{
    impl->changeRange(variable, r);
}

const std::set<std::shared_ptr<Variable> > VariableController2::variables()
{
    return impl->variables();
}

void VariableController2::synchronize(std::shared_ptr<Variable> var, std::shared_ptr<Variable> with)
{
    impl->synchronize(var, with);
}

