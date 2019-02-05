#pragma once

#include <vector>
#include <Common/typeid_cast.h>
#include <Parsers/DumpASTNode.h>

namespace DB
{

/// Visits AST tree in depth, call functions for nodes according to Matcher type data.
/// You need to define Data, label, visit() and needChildVisit() in Matcher class.
template <typename Matcher, bool _top_to_bottom>
class InDepthNodeVisitor
{
public:
    using Data = typename Matcher::Data;

    InDepthNodeVisitor(Data & data_, std::ostream * ostr_ = nullptr)
    :   data(data_),
        visit_depth(0),
        ostr(ostr_)
    {}

    void visit(ASTPtr & ast)
    {
        DumpASTNode dump(*ast, ostr, visit_depth, Matcher::label);

        if constexpr (!_top_to_bottom)
            visitChildren(ast);

        /// It operates with ASTPtr * cause we may want to rewrite ASTPtr in visit().
        std::vector<ASTPtr *> additional_nodes = Matcher::visit(ast, data);

        /// visit additional nodes (ex. only part of children)
        for (ASTPtr * node : additional_nodes)
            visit(*node);

        if constexpr (_top_to_bottom)
            visitChildren(ast);
    }

private:
    Data & data;
    size_t visit_depth;
    std::ostream * ostr;

    void visitChildren(ASTPtr & ast)
    {
        for (auto & child : ast->children)
            if (Matcher::needChildVisit(ast, child))
                visit(child);
    }
};

/// Simple matcher for one node type without complex traversal logic.
template <typename _Data, bool _visit_children = true>
class OneTypeMatcher
{
public:
    using Data = _Data;
    using TypeToVisit = typename Data::TypeToVisit;

    static constexpr const char * label = "";

    static bool needChildVisit(ASTPtr &, const ASTPtr &) { return _visit_children; }

    static std::vector<ASTPtr *> visit(ASTPtr & ast, Data & data)
    {
        if (auto * t = typeid_cast<TypeToVisit *>(ast.get()))
            data.visit(*t, ast);
        return {};
    }
};

/// Links two simple matches into resulting one. There's no complex traversal logic: all the children would be visited.
template <typename First, typename Second>
class LinkedMatcher
{
public:
    using Data = std::pair<typename First::Data, typename Second::Data>;

    static constexpr const char * label = "";

    static bool needChildVisit(ASTPtr &, const ASTPtr &) { return true; }

    static std::vector<ASTPtr *> visit(ASTPtr & ast, Data & data)
    {
        First::visit(ast, data.first);
        Second::visit(ast, data.second);
        return {};
    }
};

}
