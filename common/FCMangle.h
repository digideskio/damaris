#ifndef FC_FUNC_HEADER_INCLUDED
#define FC_FUNC_HEADER_INCLUDED

/* Mangling for Fortran global symbols without underscores. */
#define FC_FUNC_GLOBAL(name,NAME) name##_

/* Mangling for Fortran global symbols with underscores. */
#define FC_FUNC_GLOBAL_(name,NAME) name##_

/* Mangling for Fortran module symbols without underscores. */
#define FC_FUNC_MODULE(mod_name,name, mod_NAME,NAME) mod_name##_mp_##name##_

/* Mangling for Fortran module symbols with underscores. */
#define FC_FUNC_MODULE_(mod_name,name, mod_NAME,NAME) mod_name##_mp_##name##_

#endif
