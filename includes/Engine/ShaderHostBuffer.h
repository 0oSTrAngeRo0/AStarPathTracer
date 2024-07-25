#pragma once

#include "Engine/HostBuffer.h"
#include "Utilities/Singleton.h"

template <typename T>
class ShaderHostBuffer : public HostBuffer<T>, public Singleton<ShaderHostBuffer<T>> {

};