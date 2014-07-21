#ifndef STRUCTPROPS_H
#define STRUCTPROPS_H

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

*******************************************************************************************/

#include <ossie/CorbaUtils.h>

struct SRI_struct {
    SRI_struct ()
    {
        mode = false;
        xdelta = 1.0;
        subsize = 0;
    };

    static std::string getId() {
        return std::string("SRI");
    };

    bool mode;
    double xdelta;
    CORBA::ULong subsize;
};

inline bool operator>>= (const CORBA::Any& a, SRI_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("mode", props[idx].id)) {
            if (!(props[idx].value >>= s.mode)) return false;
        }
        else if (!strcmp("xdelta", props[idx].id)) {
            if (!(props[idx].value >>= s.xdelta)) return false;
        }
        else if (!strcmp("subsize", props[idx].id)) {
            if (!(props[idx].value >>= s.subsize)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const SRI_struct& s) {
    CF::Properties props;
    props.length(3);
    props[0].id = CORBA::string_dup("mode");
    props[0].value <<= s.mode;
    props[1].id = CORBA::string_dup("xdelta");
    props[1].value <<= s.xdelta;
    props[2].id = CORBA::string_dup("subsize");
    props[2].value <<= s.subsize;
    a <<= props;
};

inline bool operator== (const SRI_struct& s1, const SRI_struct& s2) {
    if (s1.mode!=s2.mode)
        return false;
    if (s1.xdelta!=s2.xdelta)
        return false;
    if (s1.subsize!=s2.subsize)
        return false;
    return true;
};

inline bool operator!= (const SRI_struct& s1, const SRI_struct& s2) {
    return !(s1==s2);
};

#endif // STRUCTPROPS_H
