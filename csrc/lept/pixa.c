#include <allheaders.h>
#include <string.h>
#include "cutils.h"
#include "quickjs.h"

static JSValue pixa_object(JSContext *ctx, PIXA *pixa) {
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "asdf", JS_NewInt32(ctx, 1234));
  JS_SetPropertyStr(ctx, obj, "n", JS_NewInt32(ctx, pixa->n));
  JS_SetPropertyStr(ctx, obj, "nalloc", JS_NewInt32(ctx, pixa->nalloc));
  JSValue pixarr = JS_NewArray(ctx);
  JS_SetPropertyStr(ctx, obj, "pix", pixarr);
  for (int i = 0; i < pixa->n; i++) {
    PIX *pix = pixaGetPix(pixa, i, L_CLONE);
    JS_SetPropertyUint32(ctx, pixarr, i, pix_object(ctx, pix));
    pixDestroy(&pix);
  }
  return obj;
}

static JSValue js_lept_pixa_create(
    JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int max = 0;
  if (argc > 0) JS_ToInt32(ctx, &max, argv[0]);
  PIXA *pixa = pixaCreate(max);
  return JS_NewInt64(ctx, (int64_t)pixa);
}

static JSValue js_lept_pixa_destroy(
    JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  if (argc == 0) return JS_EXCEPTION;
  int64_t ptrval;
  JS_ToInt64(ctx, &ptrval, argv[0]);
  PIXA *pixa = (PIXA *)ptrval;
  pixaDestroy(&pixa);
  return JS_NewInt32(ctx, 0);
}

static JSValue js_lept_pixa_describe(
    JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  if (argc == 0) return JS_EXCEPTION;
  int64_t ptrval;
  JS_ToInt64(ctx, &ptrval, argv[0]);
  PIXA *pixa = (PIXA *)ptrval;

  JSValue obj = pixa_object(ctx, pixa);
  return obj;
}

static const JSCFunctionListEntry js_lept_pixa_funcs[] = {
    JS_CFUNC_DEF("create", 2, js_lept_pixa_create),
    JS_CFUNC_DEF("destroy", 2, js_lept_pixa_destroy),
    JS_CFUNC_DEF("describe", 2, js_lept_pixa_describe),
};
