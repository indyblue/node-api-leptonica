#include "tools.c"
#include <allheaders.h>
#include <node_api.h>
#include <pix_internal.h>

#ifndef _napi_lept_box_
#define _napi_lept_box_

// static napi_status napi_get_named_box(
//     napi_env env, napi_value obj, BOX **value) {
//   void *val2;
//   napi_status status = napi_cget_ptr(env, obj, &val2);
//   *value = val2;
//   return status;
// }

// static napi_status napi_get_named_boxa(
//     napi_env env, napi_value obj, BOXA **value) {
//   void *val2;
//   napi_status status = napi_cget_ptr(env, obj, &val2);
//   *value = val2;
//   return status;
// }

static void napi_box_finalize(napi_env env, void *obj, void *hint) {
  printf("destroy box %p\n", obj);
  boxDestroy((BOX **)&obj);
}

static void napi_boxa_finalize(napi_env env, void *obj, void *hint) {
  printf("destroy boxa %p\n", obj);
  boxaDestroy((BOXA **)&obj);
}

static napi_value napi_box_object_nop(napi_env env, BOX *box) {
  napi_value obj;
  napi_create_object(env, &obj);
  napi_nset_named_uint32(env, obj, "x", box->x);
  napi_nset_named_uint32(env, obj, "y", box->y);
  napi_nset_named_uint32(env, obj, "w", box->w);
  napi_nset_named_uint32(env, obj, "h", box->h);
  return obj;
}

static napi_value napi_box_object(napi_env env, BOX *box) {
  napi_value obj = napi_box_object_nop(env, box);
  napi_nset_ptr(env, obj, box, napi_box_finalize);
  return obj;
}

static void napi_boxa_array_existing(
    napi_env env, BOXA *boxa, napi_value *arrptr) {
  napi_value obj = *arrptr;
  int cnt = boxaGetCount(boxa);
  for (int i = 0; i < cnt; i++) {
    BOX *box = boxaGetBox(boxa, i, L_CLONE);
    napi_set_element(env, obj, i, napi_box_object_nop(env, box));
    boxDestroy(&box);
  }
  napi_nset_ptr(env, obj, boxa, napi_boxa_finalize);
  napi_nset_named_uint32(env, obj, "nalloc", boxa->nalloc);
}

static napi_value napi_boxa_array(napi_env env, BOXA *boxa) {
  napi_value obj;
  napi_create_array(env, &obj);
  napi_boxa_array_existing(env, boxa, &obj);
  return obj;
}

static napi_value napi_box_destroy(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  if (napi_cget_is_array(env, args[0])) {
    BOXA *boxa = napi_cget_ptr(env, args[0]);
    boxaDestroy(&boxa);
  } else {
    BOX *box = napi_cget_ptr(env, args[0]);
    boxDestroy(&box);
  }
  napi_cdel_ptr(env, args[0]);
  return napi_nget_int32(env, 1);
}

static napi_value napi_boxa_get(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  BOXA *boxa = napi_cget_ptr(env, args[0]);
  int32_t idx = napi_cget_int32(env, args[1]);
  BOX *box = boxaGetBox(boxa, idx, L_CLONE);

  return napi_box_object(env, box);
}

static napi_value napi_box_exports(napi_env env) {
  napi_value obj;
  napi_create_object(env, &obj);
  napi_property_descriptor descr[] = {
      DECLARE_NAPI_METHOD("destroy", napi_box_destroy),
      DECLARE_NAPI_METHOD("aget", napi_boxa_get)
      // end
  };
  napi_define_properties(env, obj, sizeof(descr) / sizeof(descr[0]), descr);
  return obj;
}

#endif