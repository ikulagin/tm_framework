#include "gcc-plugin.h"
#include "config.h"
#include "coretypes.h"
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
#include "gimple-ssa.h"
#include "tree-phinodes.h"
#include "ssa-iterators.h"
#include "tree-ssanames.h"

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

// tree type = TREE_TYPE(arg);
// tcode = TREE_CODE(type);
// printf(" -> arg.type code = %s ", get_tree_code_name(tcode));
// if (tcode == POINTER_TYPE) {
//     type = TREE_TYPE(type);
//     tcode = TREE_CODE(type);
//     printf(" -> arg.type[pointer].type code = %s ", get_tree_code_name(tcode));
// }
// print_generic_expr(stdout, type, TDF_RAW | TDF_DETAILS);
// printf(" | ");
// print_generic_expr(stdout, arg, TDF_RAW | TDF_DETAILS);        
//        if (type != NULL_TREE)
//            printf("type = %s", type_name(arg));

// while (arg && (TREE_CODE(arg) == SSA_NAME)) {
//     gimple ssa_stmt = SSA_NAME_DEF_STMT(arg);
//     printf(" def_stmt_code = %s\n", gimple_code_name[gimple_code(ssa_stmt)]);
//     arg = NULL;
// }


// tree type = TREE_TYPE(arg);
// if (type == NULL)
//     printf("type is NULL ");
// print_generic_expr(stdout, type, TDF_RAW | TDF_DETAILS);

void print_arg(tree arg)
{
    if (arg == NULL) {
        printf("NULL ");
        return ;
    }
    print_generic_stmt(stdout, arg, TDF_RAW | TDF_DETAILS);
}

void process_arg(tree arg)
{
    if (arg == NULL)
        return;
        
    enum tree_code tcode = TREE_CODE(arg);
    printf(" -> arg.type code = %s ", get_tree_code_name(tcode));
 
    if (tcode == SSA_NAME) {
        gimple stmt = SSA_NAME_DEF_STMT(arg);
        if (is_gimple_assign (stmt)) {
            tree right = gimple_assign_rhs1(stmt);
            process_arg(right);
        } else {
            printf("no assign [%s] ", gimple_code_name[gimple_code(stmt)]);
        }
    } else if (tcode == ADDR_EXPR) {
        tree x = TREE_OPERAND(arg, 0);
        process_arg(x);
    } else if (tcode == COMPONENT_REF){
        print_arg(arg);
        tree zero = TREE_OPERAND(arg, 0);
        printf("\n\tzero ");
        print_arg(zero);
        process_arg(zero);
        tree one = TREE_OPERAND(arg, 1);
        printf("\n\tone ");
        print_arg(one);
        process_arg(one);
        tree two = TREE_OPERAND(arg, 2);
        printf("\n\ttwo ");
        print_arg(two);
        process_arg(two);
    }
}

unsigned int 
pass_tm_analyze::execute(struct function *fun)
{
    basic_block bb;
    gimple_stmt_iterator gsi;

    // unsigned ssa_names_count = SSANAMES(fun)->length();

    // for (unsigned i = 0; i < ssa_names_count; i++) {
    //     printf("ssa_name[%u] = ", i);
    //     print_generic_expr(stdout, (*SSANAMES(fun))[i], TDF_RAW | TDF_DETAILS);
    //     printf("\n");
    // }


    printf("%s entry\n", function_name(fun));
    FOR_EACH_BB_FN(bb, fun) {
        for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
            gimple stmt = gsi_stmt(gsi);
            enum gimple_code stmt_code = gimple_code(stmt);
            if (stmt_code == GIMPLE_CALL) {
                tree fn_decl = gimple_call_fndecl(stmt);
                if (!fn_decl ||
                    (DECL_BUILT_IN_CLASS (fn_decl) != BUILT_IN_NORMAL))
                    continue;

                int fn_code = DECL_FUNCTION_CODE(fn_decl);
                if (!BUILTIN_TM_LOAD_P(fn_code) && !BUILTIN_TM_STORE_P(fn_code))
                    continue;
                gcall* tm_call = as_a<gcall *>(stmt);
                printf("find\n");
                print_gimple_stmt(stdout, tm_call, 0, 0);

                tree arg = gimple_call_arg(tm_call, 0);
                printf("arg code %s | ", get_tree_code_name(TREE_CODE(arg)));
                print_generic_stmt(stdout, arg, TDF_RAW | TDF_DETAILS);
                printf("\n");
                process_arg(arg);
                printf("\n");
                // ssa_op_iter i;
                // use_operand_p use_p;
                // def_operand_p def_p;
                // FOR_EACH_SSA_USE_OPERAND (use_p, stmt, i, SSA_OP_ALL_USES) {
                //     tree use = USE_FROM_PTR (use_p);
                //     printf("%s:%d use ", __func__, __LINE__);
                //     if (!is_gimple_reg(use))
                //         printf("virtual ");
                //     else
                //         printf("real ");
                //     printf("%s ", get_tree_code_name(TREE_CODE(use)));
                //     print_generic_expr(stdout, use, TDF_RAW | TDF_DETAILS);
                //     gimple def_stmt = SSA_NAME_DEF_STMT(use);
                //     printf(" | def stmt code = %s", gimple_code_name[gimple_code(def_stmt)]);
                //     printf("\n");
                //     process_arg(use);
                //     printf("\n");
                // }

                // FOR_EACH_SSA_DEF_OPERAND (def_p, stmt, i, SSA_OP_ALL_DEFS) {
                //     tree def = DEF_FROM_PTR (def_p);
                //     printf("%s:%d def ", __func__, __LINE__);
                //     if (!is_gimple_reg(def))
                //         printf("virtual ");
                //     else
                //         printf("real ");
                //     printf("%s ", get_tree_code_name(TREE_CODE(def)));
                //     print_generic_expr(stdout, def, TDF_RAW | TDF_DETAILS);
                //     printf("\n");
                // }            
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
