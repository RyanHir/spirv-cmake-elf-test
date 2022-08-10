#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <spirv_cross_c.h>

#define OBJCOPY_FILE(name)                                                     \
  extern const unsigned char _binary_##name##_start[];                         \
  extern const unsigned char _binary_##name##_end[];
#define OBJCOPY_FILE_PTR(name) ((const void *)(_binary_##name##_start))
#define OBJCOPY_FILE_BYTESIZE(name)                                            \
  (_binary_##name##_end - _binary_##name##_start)
#define OBJCOPY_FILE_WORDSIZE(name)                                            \
  OBJCOPY_FILE_BYTESIZE(name) / sizeof(uint32_t)
OBJCOPY_FILE(sample_comp_glsl_spv)
OBJCOPY_FILE(sample_vert_glsl_spv)
OBJCOPY_FILE(sample_frag_glsl_spv)

static void spvc_error_callback_aaaa(void *userdata, const char *error) {
  puts(error);
  exit(EXIT_FAILURE);
}

int main() {
  const void *spirv = OBJCOPY_FILE_PTR(sample_comp_glsl_spv);
  size_t spirv_size = OBJCOPY_FILE_WORDSIZE(sample_comp_glsl_spv);

  spvc_context context = NULL;
  spvc_parsed_ir ir = NULL;
  spvc_compiler compiler_glsl = NULL;
  spvc_compiler_options options = NULL;
  spvc_resources resources = NULL;
  const spvc_reflected_resource *list = NULL;
  const char *result = NULL;
  size_t count;
  size_t i;

  // Create context.
  spvc_context_create(&context);

  // Set debug callback.
  spvc_context_set_error_callback(context, spvc_error_callback_aaaa, NULL);

  // Parse the SPIR-V.
  spvc_context_parse_spirv(context, spirv, spirv_size, &ir);

  // Hand it off to a compiler instance and give it ownership of the IR.
  spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir,
                               SPVC_CAPTURE_MODE_TAKE_OWNERSHIP,
                               &compiler_glsl);

  // Do some basic reflection.
  spvc_compiler_create_shader_resources(compiler_glsl, &resources);
  spvc_resources_get_resource_list_for_type(
      resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &list, &count);

  // Modify options.
  spvc_compiler_create_compiler_options(compiler_glsl, &options);
  spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION,
                                 330);
  spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES,
                                 SPVC_FALSE);
  spvc_compiler_install_compiler_options(compiler_glsl, options);

  spvc_compiler_compile(compiler_glsl, &result);
  printf("Cross-compiled source: \n%s\n", result);

  // Frees all memory we allocated so far.
  spvc_context_destroy(context);
}