#pragma once

#define MacroIEvent(name)                                   \
    public:                                                 \
        virtual void Connect##name(slot_type On##name) = 0; \

#define MacroEvent(name)                               \
    private:                                           \
        signal Signal##name;                           \
    public:                                            \
        virtual void Connect##name(slot_type On##name) \
        {                                              \
            Signal##name.connect(On##name);            \
        }                                              \

#define MacroTestEvent(name)                           \
    public:                                            \
        signal Signal##name;                           \
        virtual void Connect##name(slot_type On##name) \
        {                                              \
            Signal##name.connect(On##name);            \
        }                                              \

