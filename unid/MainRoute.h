#pragma once
#include "RegisterComponent.h"
#include "Dispatcher.h"

namespace unid {

using ConsumerRegistry = ComponentRegistry<IEventConsumer>;

template <typename EvtConsumer>
using RegisterConsumer = RegisterComponent<IEventConsumer, EvtConsumer>;

}
