20211224 :  5.41
20211230 :  4.55
20220217 : 19.01


Type files that still have to be cleaned from cereal/cereal.hpp include:
    !!! memory (transitive via cereal/traits/shared_from_this.hpp)
    bitset
    base_class
    polymorphic
    common (archives will need cereal.hpp anyways, common is included after each archive (not by user), so this one is not that important)


rename cereal, name ideas:
    cerealv
    cerv      (best?)
    cev
    serialv
    ver
    vereal
    verial
    vreal

