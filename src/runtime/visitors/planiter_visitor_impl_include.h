  class xqpStringStore;
  class PlanIterator;
  class SequentialIterator;
  class FlowCtlIterator;
  class LoopIterator;
  class FnDataIterator;
  class FnErrorIterator;
  class FnResolveUriIterator;
  class FnBooleanIterator;
#ifndef ZORBA_NO_XMLSCHEMA
  class ValidateIterator;
#endif
  class LogicIterator;
  class CompareIterator;
  class UDFunctionCallIterator;
  class StatelessExtFunctionCallIterator;
  class ElementIterator;
  class AttributeIterator;
  class CommentIterator;
  class PiIterator;
  class RefIterator;
  class SingletonIterator;
  class EmptyIterator;
  class ForVarIterator;
  typedef rchandle<ForVarIterator> ForVarIter_t;
  class LetVarIterator;
  typedef rchandle<LetVarIterator> LetVarIter_t;
  class EnclosedIterator;
  class IfThenElseIterator;
  class TryCatchIterator;
  class NodeDistinctIterator;
  class NodeSortIterator;
  class AxisIteratorHelper;
  template <class Object, class State> class AxisIterator;
  class SelfAxisIterator;
  class AttributeAxisIterator;
  class ParentAxisIterator;
  class AncestorAxisIterator;
  class AncestorReverseAxisIterator;
  class AncestorSelfAxisIterator;
  class AncestorSelfReverseAxisIterator;
  class RSiblingAxisIterator;
  class LSiblingAxisIterator;
  class LSiblingReverseAxisIterator;
  class ChildAxisIterator;
  class DescendantAxisIterator;
  class DescendantSelfAxisIterator;
  class PrecedingAxisIterator;
  class PrecedingReverseAxisIterator;
  class FollowingAxisIterator;
  class PathIterator;
  class InstanceOfIterator;
  class TreatIterator;
  class EitherNodesOrAtomicsIterator;
  class AddOperation;
  class SubtractOperation;
  class MultiplyOperation;
  class DivideOperation;
  class IntegerDivideOperation;
  class ModOperation;
  template < class AddOperation> class NumArithIterator;
  template < class SubtractOperation> class NumArithIterator;
  template < class MultiplyOperation> class NumArithIterator;
  template < class DivideOperation> class NumArithIterator;
  template < class IntegerDivideOperation> class NumArithIterator;
  template < class ModOperation> class NumArithIterator;
  template < class AddOperation> class GenericArithIterator;
  template < class SubtractOperation> class GenericArithIterator;
  template < class MultiplyOperation> class GenericArithIterator;
  template < class DivideOperation> class GenericArithIterator;
  template < class IntegerDivideOperation> class GenericArithIterator;
  template < class ModOperation> class GenericArithIterator;
  template < class AddOperation, TypeConstants::atomic_type_code_t > class SpecificNumArithIterator;
  template < class SubtractOperation, TypeConstants::atomic_type_code_t > class SpecificNumArithIterator;
  template < class MultiplyOperation, TypeConstants::atomic_type_code_t > class SpecificNumArithIterator;
  template < class DivideOperation, TypeConstants::atomic_type_code_t > class SpecificNumArithIterator;
  template < TypeConstants::atomic_type_code_t > class TypedValueCompareIterator;

  class CreateInternalIndexIterator;
  class CreateIndexIterator;
  class RefreshIndexIterator;
  class DropIndexIterator;
  class IndexEntryBuilderIterator;
  class IndexPointProbeIterator;
  class IndexRangeProbeIterator;

  class OpNumericUnaryIterator;
  class FnAbsIterator;
  class FnCeilingIterator;
  class FnFloorIterator;
  class FnRoundIterator;
  class FnRoundHalfToEvenIterator;
  class FnSQRTIterator;
  class FnExpIterator;
  class FnLogIterator;
  class FnSinIterator;
  class FnCosIterator;
  class FnTanIterator;
  class FnArcSinIterator;
  class FnArcCosIterator;
  class FnArcTanIterator;
  class FnIdIterator;
  class FnIdRefIterator;
  class FnDistinctValuesIterator;
  class FnMinMaxIterator;
  class FnTraceIterator;
  class FnReadStringIterator;
  class FnPrintIterator;
  class ResolveQNameIterator;
  class QNameIterator;
  class QNameEqualIterator;
  class PrefixFromQNameIterator;
  class LocalNameFromQNameIterator;
  class NamespaceUriFromQNameIterator;
  class NamespaceUriForPrefixIterator;
  class InScopePrefixesIterator;
  class StringToCodepointsIterator;
  class FnTokenizeIterator;
  class ZorNumGen;
  class TextIterator;
  class DocumentIterator;
  class DocumentContentIterator;
  class CastIterator;
  class NameCastIterator;
  class CastableIterator;
  class PromoteIterator;
  class CtxVariableIterator;
  class CtxVarDeclIterator;
  class CtxVarAssignIterator;
  class CtxVarExistsIterator;
  class EvalIterator;
  class FnDateTimeConstructorIterator;
  class FnYearsFromDurationIterator;
  class FnMonthsFromDurationIterator;
  class FnDaysFromDurationIterator;
  class FnHoursFromDurationIterator;
  class FnMinutesFromDurationIterator;
  class FnSecondsFromDurationIterator;
  class FnYearFromDatetimeIterator;
  class FnMonthFromDatetimeIterator;
  class FnDayFromDatetimeIterator;
  class FnHoursFromDatetimeIterator;
  class FnMinutesFromDatetimeIterator;
  class FnSecondsFromDatetimeIterator;
  class FnTimezoneFromDatetimeIterator;
  class FnYearFromDateIterator;
  class FnMonthFromDateIterator;
  class FnDayFromDateIterator;
  class FnTimezoneFromDateIterator;
  class FnHoursFromTimeIterator;
  class FnMinutesFromTimeIterator;
  class FnSecondsFromTimeIterator;
  class FnTimezoneFromTimeIterator;
  class FnAdjustToTimeZoneIterator_1;
  class FnAdjustToTimeZoneIterator_2;
  class FnFormatDateTimeIterator;
  class InsertIterator;
  class DeleteIterator;
  class ReplaceIterator;
  class RenameIterator;
  class TransformIterator;
  class ApplyIterator;
  class NodeReferenceIterator;
  class NodeByReferenceIterator;
  class FnLocalNameIterator;
  class FnNamespaceUriIterator;
  class FnLangIterator;
  class FnCollectionIterator;
  class CreateTupleIterator;
  class GetTupleFieldIterator;
  class FnFormatNumberIterator;

  class ZorbaCollectionExistsIterator;
  class ZorbaImportXmlIterator;
  class ZorbaImportCatalogIterator;
  class ZorbaImportFolderIterator;
  class ZorbaListCollectionsIterator;
  class ZorbaCreateCollectionIterator;
  class ZorbaDeleteCollectionIterator;
  class ZorbaDeleteAllCollectionsIterator;
  class ZorbaInsertNodeFirstIterator;
  class ZorbaInsertNodeLastIterator;
  class ZorbaInsertNodeBeforeIterator;
  class ZorbaInsertNodeAfterIterator;
  class ZorbaInsertNodeAtIterator;
  class ZorbaRemoveNodeIterator;
  class ZorbaRemoveNodeAtIterator;
  class ZorbaNodeCountIterator;
  class ZorbaNodeAtIterator;
  class ZorbaIndexOfIterator;
  class ZorbaExportXmlIterator;

  class ZorbaSchemaTypeIterator;
  class ZorbaBase64EncodeIterator;
  class ZorbaBase64DecodeIterator;
  class XQDocIterator;
#ifdef ZORBA_WITH_TIDY
  class ZorbaTidyIterator;
  class ZorbaTDocIterator;
#endif  /* ZORBA_WITH_TIDY */
  class ZorbaRandomIterator;
  class ZorbaUUIDIterator;
#ifdef ZORBA_WITH_EMAIL
  class ZorbaMailIterator;
#endif  /* ZORBA_WITH_EMAIL */
#ifdef ZORBA_WITH_FOP
  class ZorbaFopIterator;
#endif

  class ZorbaTimestampIterator;

  namespace flwor
  {
    class FLWORIterator;
    class TupleStreamIterator;
    class TupleSourceIterator;
    class ForIterator;
    class OuterForIterator;
    class LetIterator;
    class WhereIterator;
    class CountIterator;
    class GroupByIterator;
    class OrderByIterator;
    class WindowIterator;
  }

  class ZorbaDebugIterator;
  class DebuggerSingletonIterator;

#ifdef ZORBA_WITH_REST
  class ZorbaRestGetIterator;
  class ZorbaRestPostIterator;
  class ZorbaRestPutIterator;
  class ZorbaRestDeleteIterator;
  class ZorbaRestHeadIterator;
#endif

  class HoistIterator;
  class UnhoistIterator;
  
 #define PLAN_ITER_VISITOR(class)                \
  virtual void beginVisit ( const class& ) = 0;  \
  virtual void endVisit ( const class& ) = 0
 
