#include "gcc-plugin.h"
#include "c-family/c-common.h"
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "stringpool.h"
#include "toplev.h"
#include "basic-block.h"
#include "hash-table.h"
#include "vec.h"
#include "ggc.h"
#include "basic-block.h"
#include "tree-ssa-alias.h"
#include "internal-fn.h"
#include "gimple-fold.h"
#include "tree-eh.h"
#include "gimple-expr.h"
#include "is-a.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree.h"
#include "tree-pass.h"
#include "intl.h"
#include "plugin-version.h"
#include "diagnostic.h"
#include "context.h"
#include "gimple-pretty-print.h"
#include "tree-pretty-print.h"
#include "gimple-walk.h"
#include "cgraph.h"
#include "cp/name-lookup.h"

int plugin_is_GPL_compatible;

extern struct ht *ident_hash;

const pass_data pass_data_tm_analyze =
    {
        GIMPLE_PASS, /* type */
        "tm_analyzer", /* name */
        OPTGROUP_INLINE, /* optinfo_flags */
        TV_NONE, /* tv_id */
        PROP_trees, /* properties_required */
        0, /* properties_provided */
        0, /* properties_destroyed */
        0, /* todo_flags_start */
        0, /* todo_flags_finish */
    };

class pass_tm_analyze : public gimple_opt_pass
{
 public:
 pass_tm_analyze(gcc::context *ctxt)
     : gimple_opt_pass(pass_data_tm_analyze, ctxt)
        {}

    /* opt_pass methods: */
    virtual unsigned int execute (function *);
}; 

int get_size_operated_mem(int fn_code)
{
    switch(fn_code) {
    case BUILT_IN_TM_STORE_1:
    case BUILT_IN_TM_STORE_WAR_1:
    case BUILT_IN_TM_STORE_WAW_1:
    case BUILT_IN_TM_LOAD_1:
    case BUILT_IN_TM_LOAD_RAR_1:
    case BUILT_IN_TM_LOAD_RAW_1:
    case BUILT_IN_TM_LOAD_RFW_1:
        return 1;

    case BUILT_IN_TM_STORE_2:
    case BUILT_IN_TM_STORE_WAR_2:
    case BUILT_IN_TM_STORE_WAW_2:
    case BUILT_IN_TM_LOAD_2:
    case BUILT_IN_TM_LOAD_RAR_2:
    case BUILT_IN_TM_LOAD_RAW_2:
    case BUILT_IN_TM_LOAD_RFW_2:
        return 2;

    case BUILT_IN_TM_STORE_4:
    case BUILT_IN_TM_STORE_WAR_4:
    case BUILT_IN_TM_STORE_WAW_4:
    case BUILT_IN_TM_LOAD_4:
    case BUILT_IN_TM_LOAD_RAR_4:
    case BUILT_IN_TM_LOAD_RAW_4:
    case BUILT_IN_TM_LOAD_RFW_4:
        return 4;

    case BUILT_IN_TM_STORE_8:
    case BUILT_IN_TM_STORE_WAR_8:
    case BUILT_IN_TM_STORE_WAW_8:
    case BUILT_IN_TM_LOAD_8:
    case BUILT_IN_TM_LOAD_RAR_8:
    case BUILT_IN_TM_LOAD_RAW_8:
    case BUILT_IN_TM_LOAD_RFW_8:
        return 8;

    case BUILT_IN_TM_STORE_FLOAT:
    case BUILT_IN_TM_STORE_WAR_FLOAT:
    case BUILT_IN_TM_STORE_WAW_FLOAT:
    case BUILT_IN_TM_LOAD_FLOAT:
    case BUILT_IN_TM_LOAD_RAR_FLOAT:
    case BUILT_IN_TM_LOAD_RAW_FLOAT:
    case BUILT_IN_TM_LOAD_RFW_FLOAT:
        return sizeof(float);

    case BUILT_IN_TM_STORE_DOUBLE:
    case BUILT_IN_TM_STORE_WAR_DOUBLE:
    case BUILT_IN_TM_STORE_WAW_DOUBLE:
    case BUILT_IN_TM_LOAD_DOUBLE:
    case BUILT_IN_TM_LOAD_RAR_DOUBLE:
    case BUILT_IN_TM_LOAD_RAW_DOUBLE:
    case BUILT_IN_TM_LOAD_RFW_DOUBLE:
        return sizeof(double);

    case BUILT_IN_TM_STORE_LDOUBLE:
    case BUILT_IN_TM_STORE_WAR_LDOUBLE:
    case BUILT_IN_TM_STORE_WAW_LDOUBLE:
    case BUILT_IN_TM_LOAD_LDOUBLE:
    case BUILT_IN_TM_LOAD_RAR_LDOUBLE:
    case BUILT_IN_TM_LOAD_RAW_LDOUBLE:
    case BUILT_IN_TM_LOAD_RFW_LDOUBLE:
        return sizeof(long double);

    case BUILT_IN_TM_STORE_M64:
    case BUILT_IN_TM_STORE_WAR_M64:
    case BUILT_IN_TM_STORE_WAW_M64:
    case BUILT_IN_TM_LOAD_M64:
    case BUILT_IN_TM_LOAD_RAR_M64:
    case BUILT_IN_TM_LOAD_RAW_M64:
    case BUILT_IN_TM_LOAD_RFW_M64:
        return 64;

    case BUILT_IN_TM_STORE_M128:
    case BUILT_IN_TM_STORE_WAR_M128:
    case BUILT_IN_TM_STORE_WAW_M128:
    case BUILT_IN_TM_LOAD_M128:
    case BUILT_IN_TM_LOAD_RAR_M128:
    case BUILT_IN_TM_LOAD_RAW_M128:
    case BUILT_IN_TM_LOAD_RFW_M128:
        return 128;

    case BUILT_IN_TM_STORE_M256:
    case BUILT_IN_TM_STORE_WAR_M256:
    case BUILT_IN_TM_STORE_WAW_M256:
    case BUILT_IN_TM_LOAD_M256:
    case BUILT_IN_TM_LOAD_RAR_M256:
    case BUILT_IN_TM_LOAD_RAW_M256:
    case BUILT_IN_TM_LOAD_RFW_M256:
        return 256;
    }
    
    return 0;
}

gimple create_tm_prof_begin()
{
    tree fn_type = build_function_type_list(void_type_node, NULL_TREE);
    tree fn_decl = build_fn_decl("tm_prof_begin", fn_type);
    gimple stmt = gimple_build_call(fn_decl, 0);

    return stmt;
}

gimple create_tm_prof_operation(int size)
{
    tree fn_type = build_function_type_list(void_type_node, integer_type_node, NULL_TREE);
    tree size_mem_cst = build_int_cst(integer_type_node, size);
    tree fn_decl = build_fn_decl("tm_prof_operation", fn_type);
    gimple stmt = gimple_build_call(fn_decl, 1, size_mem_cst);

    return stmt;
}

unsigned int 
pass_tm_analyze::execute(struct function *fun)
{
    basic_block bb;
    gimple tm_prof_begin;
    gimple tm_prof_operation;
    gimple_stmt_iterator gsi;

    printf("%s entry\n", function_name(fun));
    FOR_EACH_BB_FN(bb, fun) {
        for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
            gimple stmt = gsi_stmt(gsi);
            enum gimple_code stmt_code = gimple_code(stmt);
            if (stmt_code == GIMPLE_CALL) {
                tree fn_decl = gimple_call_fndecl(stmt);
                if (fn_decl &&
                    DECL_BUILT_IN_CLASS (fn_decl) == BUILT_IN_NORMAL) {
                    int fn_code = DECL_FUNCTION_CODE(fn_decl);
                    switch (fn_code) {
                    case BUILT_IN_TM_START:
                        print_gimple_stmt(stderr, stmt, 0, 0);
                        tm_prof_begin = create_tm_prof_begin();
                        gsi_insert_after(&gsi, tm_prof_begin, GSI_NEW_STMT);
                        break;
                    case BUILT_IN_TM_COMMIT:
                        print_gimple_stmt(stderr, stmt, 0, 0);
                        break;
                    case BUILT_IN_TM_COMMIT_EH:
                        print_gimple_stmt(stderr, stmt, 0, 0);
                        break;
                    default:
                        if (BUILTIN_TM_LOAD_P(fn_code) || BUILTIN_TM_STORE_P(fn_code)) {
                            printf("sizeof(mem) = %d\n", get_size_operated_mem(fn_code));
                            tm_prof_operation = create_tm_prof_operation(get_size_operated_mem(fn_code));
                            gsi_insert_after(&gsi, tm_prof_operation, GSI_NEW_STMT);                            
                            print_gimple_stmt(stderr, stmt, 0, 0);
                        }
                        break;
                    }
                }

            }
        }
    }
    printf("%s exit\n", function_name(fun));

    return 0;
}

int ht_ident_cb(struct cpp_reader *reader, hashnode ht_node, const void *v)
{
    printf("%s\n", ht_node->str);

    return 1;
}

static gimple_opt_pass *
make_pass_tm_analyze(gcc::context *ctxt)
{
    return new pass_tm_analyze (ctxt);
}

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
    struct register_pass_info pass_info;

    if(!plugin_default_version_check (version, &gcc_version)) {
        printf("This plugin is meant for use with gcc %s\n", version->basever);
        return 1;
    }

    /* Self-assign detection should happen after SSA is constructed.  */
    pass_info.pass = make_pass_tm_analyze (g);
    pass_info.reference_pass_name = "tmedge";
    pass_info.ref_pass_instance_number = 1;
    pass_info.pos_op = PASS_POS_INSERT_AFTER;

    register_callback (plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);
    
    return 0;
}
