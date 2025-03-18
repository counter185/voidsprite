#pragma once

template<typename V> inline const V div_floor(V val, V di) {
    return val / di - (val < 0);
}

template<typename V> inline const V div_ceil(V val, V di) {
    return val / di + (val & 1);
}
