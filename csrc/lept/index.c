#include "box.c"
#include "pix.c"
#include <allheaders.h>
#include <node_api.h>
#include <pix_internal.h>

static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor descr[] = {
      DECLARE_NAPI_VALUE("pix", napi_pix_exports(env)),
      DECLARE_NAPI_VALUE("box", napi_box_exports(env))
      // end
  };
  napi_define_properties(env, exports, sizeof(descr) / sizeof(descr[0]), descr);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
