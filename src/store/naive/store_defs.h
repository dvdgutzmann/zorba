
#ifndef XQP_DEFAULT_STORE_DEFS
#define XQP_DEFAULT_STORE_DEFS
 
#define GET_STORE() \
        (*(static_cast<SimpleStore*>(&GENV.getStore())))

#define GET_FACTORY() \
        (*(reinterpret_cast<BasicItemFactory*>(GET_STORE().getItemFactory())))


#define BASE_NODE(item) (static_cast<XmlNode*>((item).getp()))

#define DOC_NODE(item) (static_cast<DocumentNode*>((item).getp()))

#define ELEM_NODE(item) (static_cast<ElementNode*>((item).getp()))

#define ATTR_NODE(item) (static_cast<AttributeNode*>((item).getp()))

#define TEXT_NODE(item) (static_cast<TextNode*>((item).getp()))

#define PI_NODE(item) (static_cast<PiNode*>((item).getp()))

#define COMMENT_NODE(item) (static_cast<CommentNode*>((item).getp()))

#endif
