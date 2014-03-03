/*
 * UAVCAN data structure definition for libuavcan.
 *
 * Autogenerated, do not edit.
 *
 * Source file: ${t.source_file}
 */

#pragma once

#include <uavcan/data_type.hpp>
#include <uavcan/global_data_type_registry.hpp>
#include <uavcan/internal/util.hpp>

% for inc in t.cpp_includes:
#include <${inc}>
% endfor

<%!
indent = lambda text, idnt='    ': idnt + text.replace('\n', '\n' + idnt)
%>

/******************************* Source text **********************************
% for line in t.source_text.strip().splitlines():
${line}
% endfor
******************************************************************************/

/********************* DSDL signature source definition ***********************
% for line in t.get_dsdl_signature_source_definition().splitlines():
${line}
% endfor
******************************************************************************/

% for nsc in t.cpp_namespace_components:
namespace ${nsc}
{
% endfor
namespace
{

#if UAVCAN_PACK_STRUCTS
UAVCAN_PACKED_BEGIN
#endif

struct ${t.short_name}
{
<%def name="generate_primary_body(type_name, max_bitlen, fields, constants)" buffered="True">
    typedef const ${type_name}& ParameterType;
    typedef ${type_name}& ReferenceType;

<%def name="expand_attr_types(group_name, attrs)">
    struct ${group_name}
    {
% for a in attrs:
        typedef ${a.cpp_type} ${a.name};
% endfor
    };
</%def>
    ${expand_attr_types('ConstantTypes', constants)}
    ${expand_attr_types('FieldTypes', fields)}

<%def name="expand_enum_per_field(enum_name)">
    enum
    {
        ${enum_name}
% for idx,a in enumerate(fields):
            ${'=' if idx == 0 else '+'} FieldTypes::${a.name}::${enum_name}
% endfor
    };
</%def>
    ${expand_enum_per_field('MinBitLen')}
    ${expand_enum_per_field('MaxBitLen')}

    // Constants
% for a in constants:
    % if a.cpp_use_enum:
    enum { ${a.name} = ${a.cpp_value} }; // ${a.init_expression}
    % else:
    static const typename ::uavcan::StorageType< ConstantTypes::${a.name} >::Type ${a.name}; // ${a.init_expression}
    %endif
% endfor

    // Fields
% for a in fields:
    typename ::uavcan::StorageType< FieldTypes::${a.name} >::Type ${a.name};
% endfor

    ${type_name}()
% for idx,a in enumerate(fields):
    ${':' if idx == 0 else ','} ${a.name}()
% endfor
    {
#if UAVCAN_DEBUG
        /*
         * Cross-checking MaxBitLen provided by the DSDL compiler.
         * This check shall never be performed in user code because MaxBitLen value
         * actually depends on the nested types, thus it is not invariant.
         */
        ::uavcan::StaticAssert<${max_bitlen} == MaxBitLen>::check();
#endif
    }

<%def name="generate_codec_calls_per_field(call_name, self_parameter_type)">
    static int ${call_name}(${self_parameter_type} self, ::uavcan::ScalarCodec& codec,
                      ::uavcan::TailArrayOptimizationMode tao_mode = ::uavcan::TailArrayOptEnabled)
    {
        int res = 1;
% for idx,a in enumerate(fields):
        res = FieldTypes::${a.name}::${call_name}(self.${a.name}, codec, \
${'::uavcan::TailArrayOptDisabled' if (idx + 1) < len(fields) else 'tao_mode'});
        if (res <= 0)
            return res;
% endfor
        return res;
    }
</%def>
    ${generate_codec_calls_per_field('encode', 'ParameterType')}
    ${generate_codec_calls_per_field('decode', 'ReferenceType')}
</%def>
% if t.kind == t.KIND_SERVICE:
    struct Request
    {
        ${generate_primary_body('Request', t.get_max_bitlen_request(), t.request_fields, t.request_constants) | indent}
    };

    struct Response
    {
        ${generate_primary_body('Response', t.get_max_bitlen_response(), t.response_fields, t.response_constants) | indent}
    };
% else:
    ${generate_primary_body(t.short_name, t.get_max_bitlen(), t.fields, t.constants)}
% endif

    /*
     * Static type info
     */
    enum { DataTypeKind = ${t.cpp_kind} };
% if t.has_default_dtid:
    enum { DefaultDataTypeID = ${t.default_dtid} };
% else:
    // This type has no default data type ID
% endif

    static const char* getDataTypeFullName()
    {
        return "${t.full_name}";
    }

    static void extendDataTypeSignature(::uavcan::DataTypeSignature& signature)
    {
        signature.extend(getDataTypeSignature());
    }

    static ::uavcan::DataTypeSignature getDataTypeSignature()
    {
        ::uavcan::DataTypeSignature signature(${hex(t.get_dsdl_signature())}UL);
<%def name="extend_signature_per_field(scope_prefix, fields)">
    % for a in fields:
        ${scope_prefix}FieldTypes::${a.name}::extendDataTypeSignature(signature);
    % endfor
</%def>
% if t.kind == t.KIND_SERVICE:
    ${extend_signature_per_field('Request::', t.request_fields)}
    ${extend_signature_per_field('Response::', t.response_fields)}
% else:
    ${extend_signature_per_field('', t.fields)}
% endif
        return signature;
    }
};

<%def name="define_out_of_line_constants(scope_prefix, constants)">
% for a in constants:
    % if not a.cpp_use_enum:
const typename ::uavcan::StorageType< ${scope_prefix}::ConstantTypes::${a.name} >::Type
    ${scope_prefix}::${a.name} = ${a.cpp_value}; // ${a.init_expression}
    %endif
% endfor
</%def>
% if t.kind == t.KIND_SERVICE:
${define_out_of_line_constants(t.short_name + '::Request', t.request_constants)}
${define_out_of_line_constants(t.short_name + '::Response', t.response_constants)}
% else:
${define_out_of_line_constants(t.short_name, t.constants)}
% endif

#if UAVCAN_PACK_STRUCTS
UAVCAN_PACKED_END
#endif

// TODO Stream operator

% if t.has_default_dtid:
const ::uavcan::DefaultDataTypeRegistrator< ${t.short_name} > _uavcan_gdtr_registrator_${t.short_name};
% else:
// No default registration
% endif

} // Anonymous namespace
% for nsc in t.cpp_namespace_components:
} // Namespace ${nsc}
% endfor

