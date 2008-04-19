#include "compiler/rewriter/rules/ruleset.h"
#include "context/static_context.h"
#include "compiler/rewriter/tools/expr_tools.h"

namespace zorba {

RULE_REWRITE_PRE(EliminateExtraneousPathSteps)
{
  relpath_expr *re = dynamic_cast<relpath_expr *>(node);
  if (re != NULL) {
    ulong numSteps = re->size();
    
    for (ulong i = 0; i < numSteps - 1; i++) {
      axis_step_expr* axisStep = dynamic_cast<axis_step_expr*>((*re)[i].getp());
      
      if (axisStep != NULL &&
          axisStep->getAxis() == axis_kind_descendant_or_self &&
          axisStep->getTest()->getTestKind() == match_anykind_test)
      {
        axis_step_expr* nextStep = dynamic_cast<axis_step_expr*>((*re)[i+1].getp());
        if (nextStep != NULL &&
            nextStep->getAxis() == axis_kind_child)
        {
          nextStep->setAxis(axis_kind_descendant);
          (*re).erase(i);
          numSteps--;
          i--;
        }
      }
    }

    if (numSteps == 1) {
      return (*re) [0];
    }
  }

  return NULL;
}

RULE_REWRITE_POST(EliminateExtraneousPathSteps)
{
  return NULL;
}

}
/* vim:set ts=2 sw=2: */
