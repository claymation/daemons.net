package com.craftinginterpreters.lox;

class RpnPrinter implements Expr.Visitor<String> {
  String print(Expr expr) {
    return expr.accept(this);
  }

  @Override
  public String visitBinaryExpr(Expr.Binary expr) {
    StringBuilder builder = new StringBuilder();

    builder.append(expr.left.accept(this));
    builder.append(" ");
    builder.append(expr.right.accept(this));
    builder.append(" ");
    builder.append(expr.operator.lexeme);

    return builder.toString();
  }

  @Override
  public String visitGroupingExpr(Expr.Grouping expr) {
    return expr.expression.accept(this);
  }

  @Override
  public String visitLiteralExpr(Expr.Literal expr) {
    if (expr.value == null) return "nil";
    return expr.value.toString();
  }

  @Override
  public String visitUnaryExpr(Expr.Unary expr) {
    return "";
  }

  public static void main(String[] args) {
    Expr expression = new Expr.Binary(
        new Expr.Grouping(
	    new Expr.Binary(
                new Expr.Literal(1),
                new Token(TokenType.PLUS, "+", null, 1),
                new Expr.Literal(2))),
        new Token(TokenType.STAR, "*", null, 1),
        new Expr.Grouping(
	    new Expr.Binary(
                new Expr.Literal(4),
                new Token(TokenType.MINUS, "-", null, 1),
                new Expr.Literal(3))));

    System.out.println(new RpnPrinter().print(expression));
  }
}
