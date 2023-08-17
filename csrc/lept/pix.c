#include "box.c"
#include "tools.c"
#include <allheaders.h>
#include <node_api.h>
#include <pix_internal.h>

// static napi_status napi_get_named_pix(
//     napi_env env, napi_value obj, PIX **value) {
//   void *val2;
//   napi_status status = napi_cget_ptr(env, obj, &val2);
//   *value = val2;
//   return status;
// }

// static napi_status napi_get_named_pixa(
//     napi_env env, napi_value obj, PIXA **value) {
//   void *val2;
//   napi_status status = napi_cget_ptr(env, obj, &val2);
//   *value = val2;
//   return status;
// }

static void napi_pix_finalize(napi_env env, void *obj, void *hint) {
  printf("destroy pix %p\n", obj);
  pixDestroy((PIX **)&obj);
}

static void napi_pixa_finalize(napi_env env, void *obj, void *hint) {
  printf("destroy pix %p\n", obj);
  pixaDestroy((PIXA **)&obj);
}

static napi_value napi_pix_object_nop(napi_env env, PIX *pix) {
  napi_value obj;
  napi_create_object(env, &obj);
  napi_nset_named_uint32(env, obj, "w", pix->w);
  napi_nset_named_uint32(env, obj, "h", pix->h);
  napi_nset_named_uint32(env, obj, "d", pix->d);
  return obj;
}

static napi_value napi_pix_object(napi_env env, PIX *pix) {
  napi_value obj = napi_pix_object_nop(env, pix);
  napi_nset_ptr(env, obj, pix, napi_pix_finalize);
  return obj;
}

static void napi_pixa_array_existing(
    napi_env env, PIXA *pixa, napi_value *arrptr) {
  napi_value obj = *arrptr;
  int cnt = pixaGetCount(pixa);
  for (int i = 0; i < cnt; i++) {
    PIX *pix = pixaGetPix(pixa, i, L_CLONE);
    napi_value iobj = napi_pix_object_nop(env, pix);
    napi_nset_named_uint32(env, iobj, "i", i);
    napi_set_element(env, obj, i, iobj);
    pixDestroy(&pix);
  }
  napi_nset_ptr(env, obj, pixa, napi_pixa_finalize);
  napi_nset_named_uint32(env, obj, "nalloc", pixa->nalloc);
}

static napi_value napi_pixa_array(napi_env env, PIXA *pixa) {
  napi_value obj;
  napi_create_array(env, &obj);
  napi_pixa_array_existing(env, pixa, &obj);
  return obj;
}

static napi_value napi_pix_open(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  char *name = napi_cget_string(env, args[0]);
  PIX *pix = pixRead(name);
  free(name);
  return napi_pix_object(env, pix);
}

static napi_value napi_pix_copy(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  PIX *pix1 = napi_cget_ptr(env, args[0]);
  PIX *pix2 = pixCopy(NULL, pix1);
  return napi_pix_object(env, pix2);
}

static napi_value napi_pix_destroy(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  if (napi_cget_is_array(env, args[0])) {
    PIXA *pixa = napi_cget_ptr(env, args[0]);
    pixaDestroy(&pixa);
  } else {
    PIX *pix = napi_cget_ptr(env, args[0]);
    pixDestroy(&pix);
  }
  napi_cdel_ptr(env, args[0]);
  return napi_nget_int32(env, 1);
}

static napi_value napi_pix_rotate(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  PIX *pix1, *pix2;
  pix1 = napi_cget_ptr(env, args[0]);
  int32_t rot = napi_cget_int32(env, args[1]);
  pix2 = pixRotateOrth(pix1, rot);
  return napi_pix_object(env, pix2);
}

PIX *pixAdaptiveThreshold(PIX *pixs, l_int32 thresh) {
  PIX *p1 = pixBackgroundNormSimple(pixs, NULL, NULL);
  PIX *p2 = pixConvertRGBToGray(p1, 0, 0, 0);
  pixDestroy(&p1);
  PIX *p3 = pixThresholdToBinary(p2, thresh);
  pixDestroy(&p2);
  return p3;
}

static napi_value napi_pix_thresh(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  PIX *pix1, *pix2;
  pix1 = napi_cget_ptr(env, args[0]);
  l_int32 thr = napi_cget_int32(env, args[1]);
  pix2 = pixAdaptiveThreshold(pix1, thr);

  return napi_pix_object(env, pix2);
}

static napi_value napi_pix_morph(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  PIX *pix1, *pix2;
  pix1 = napi_cget_ptr(env, args[0]);
  char *morph = napi_cget_string(env, args[1]);
  pix2 = pixMorphSequence(pix1, morph, 0);
  free(morph);
  return napi_pix_object(env, pix2);
}

static napi_value napi_pix_save(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  PIX *pix1 = napi_cget_ptr(env, args[0]);
  char *name = napi_cget_string(env, args[1]);
  l_int32 fmt = napi_cget_int32(env, args[2]);
  pixWrite(name, pix1, fmt);
  free(name);
  return napi_nget_int32(env, 1);
}

static napi_value napi_pix_conncomp(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  PIX *pix1 = napi_cget_ptr(env, args[0]);
  l_int32 conn = napi_cget_int32(env, args[2]);
  if (argc < 3 || conn != 4) conn = 8;
  BOXA *boxa;
  if (napi_cget_is_array(env, args[1])) {
    PIXA *pixa;
    boxa = pixConnComp(pix1, &pixa, conn);
    napi_pixa_array_existing(env, pixa, &args[1]);
  } else {
    boxa = pixConnComp(pix1, NULL, conn);
  }
  return napi_boxa_array(env, boxa);
}

static napi_value napi_pixa_get(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  PIXA *pixa = napi_cget_ptr(env, args[0]);
  int32_t idx = napi_cget_int32(env, args[1]);
  PIX *pix = pixaGetPix(pixa, idx, L_CLONE);

  return napi_pix_object(env, pix);
}

// pixConnComp
static napi_value napi_save_types(napi_env env) {
  napi_value obj;
  napi_create_object(env, &obj);
  napi_nset_named_uint32(env, obj, "bmp", 1);
  napi_nset_named_uint32(env, obj, "jpeg", 2);
  napi_nset_named_uint32(env, obj, "jfif_jpeg", 2);
  napi_nset_named_uint32(env, obj, "png", 3);
  napi_nset_named_uint32(env, obj, "tiff", 4);
  napi_nset_named_uint32(env, obj, "tiff_packbits", 5);
  napi_nset_named_uint32(env, obj, "tiff_rle", 6);
  napi_nset_named_uint32(env, obj, "tiff_g3", 7);
  napi_nset_named_uint32(env, obj, "tiff_g4", 8);
  napi_nset_named_uint32(env, obj, "tiff_lzw", 9);
  napi_nset_named_uint32(env, obj, "tiff_zip", 10);
  napi_nset_named_uint32(env, obj, "pnm", 11);
  napi_nset_named_uint32(env, obj, "ps", 12);
  napi_nset_named_uint32(env, obj, "gif", 13);
  napi_nset_named_uint32(env, obj, "jp2", 14);
  napi_nset_named_uint32(env, obj, "webp", 15);
  napi_nset_named_uint32(env, obj, "lpdf", 16);
  napi_nset_named_uint32(env, obj, "tiff_jpeg", 17);
  napi_nset_named_uint32(env, obj, "default", 18);
  napi_nset_named_uint32(env, obj, "spix", 19);
  return obj;
}

static napi_value napi_pix_exports(napi_env env) {
  napi_value obj;
  napi_create_object(env, &obj);
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_METHOD("open", napi_pix_open),
      DECLARE_NAPI_METHOD("copy", napi_pix_copy),
      DECLARE_NAPI_METHOD("rotate", napi_pix_rotate),
      DECLARE_NAPI_METHOD("thresh", napi_pix_thresh),
      DECLARE_NAPI_METHOD("morph", napi_pix_morph),
      DECLARE_NAPI_METHOD("connComp", napi_pix_conncomp),
      DECLARE_NAPI_METHOD("aget", napi_pixa_get),
      DECLARE_NAPI_METHOD("save", napi_pix_save),
      DECLARE_NAPI_VALUE("formats", napi_save_types(env)),
      DECLARE_NAPI_METHOD("destroy", napi_pix_destroy)
      // end
  };
  napi_define_properties(env, obj, sizeof(desc) / sizeof(desc[0]), desc);
  return obj;
}