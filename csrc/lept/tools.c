#include <allheaders.h>
#include <node_api.h>
#include <pix_internal.h>
#include <string.h>

#ifndef _napi_lept_tools_
#define _napi_lept_tools_

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

#define DECLARE_NAPI_VALUE(name, val)                                          \
  { name, 0, 0, 0, 0, val, napi_default, 0 }

static const char *ptrkey = "ptr";
static const char *ptrrefkey = "_ptrref";
static napi_status _napi_nset_named_ptr(napi_env env, napi_value obj,
    const char *key, void *value, napi_finalize cb) {
  napi_status status;
  napi_value nvalue;
  status = napi_create_external(env, value, NULL, NULL, &nvalue);
  if (cb) { 
    // ref could be created automatically by create_external, but this way
    // we can delete the finalize ref when destroying.
    napi_ref finref;
    napi_add_finalizer(env, nvalue, value, cb, NULL, &finref);
    _napi_nset_named_ptr(env, obj, ptrrefkey, finref, NULL);
  }
  if (status != napi_ok) return status;
  status = napi_set_named_property(env, obj, key, nvalue);
  return status;
}

static napi_status napi_nset_ptr(
    napi_env env, napi_value obj, void *value, napi_finalize cb) {
  return _napi_nset_named_ptr(env, obj, ptrkey, value, cb);
}

static void *napi_cget_named_ptr(
    napi_env env, napi_value obj, const char *key) {
  napi_value nvalue;
  void *val;
  napi_get_named_property(env, obj, key, &nvalue);
  napi_get_value_external(env, nvalue, &val);
  return val;
}

static void *napi_cget_ptr(napi_env env, napi_value obj) {
  return napi_cget_named_ptr(env, obj, ptrkey);
}

static napi_ref napi_cget_ptrref(napi_env env, napi_value obj) {
  return napi_cget_named_ptr(env, obj, ptrrefkey);
}

static napi_status napi_nset_named_uint32(
    napi_env env, napi_value obj, const char *key, uint32_t value) {
  napi_status status;
  napi_value nvalue;
  status = napi_create_uint32(env, value, &nvalue);
  if (status != napi_ok) return status;
  status = napi_set_named_property(env, obj, key, nvalue);
  return status;
}

static char *napi_cget_string(napi_env env, napi_value nval) {
  size_t buffer_size = 2048;
  char buffer[buffer_size];
  size_t copied;
  napi_get_value_string_latin1(env, nval, buffer, buffer_size, &copied);
  char *rv = malloc((copied + 1) * sizeof(char));
  strcpy(rv, buffer);
  return rv;
}

static int32_t napi_cget_int32(napi_env env, napi_value nval) {
  int32_t val;
  napi_get_value_int32(env, nval, &val);
  return val;
}

static napi_value napi_nget_int32(napi_env env, int32_t value) {
  napi_value rv;
  napi_create_int32(env, value, &rv);
  return rv;
}

static bool napi_cget_is_array(napi_env env, napi_value nval) {
  bool rv;
  napi_is_array(env, nval, &rv);
  return rv;
}

static bool napi_cdel_ptr(napi_env env, napi_value obj) {
  bool rv;
  napi_value pkey, rkey;
  
  // reference, cleans up the finalize
  napi_ref ref = napi_cget_ptrref(env, obj);
  napi_delete_reference(env, ref);
  napi_create_string_latin1(env, ptrrefkey, 3, &rkey);
  napi_delete_property(env, obj, rkey, &rv);
  
  // then remove the ptr
  napi_create_string_latin1(env, ptrkey, 3, &pkey);
  napi_has_property(env, obj, pkey, &rv);
  if (rv) napi_delete_property(env, obj, pkey, &rv);
  return rv;
}

#endif