# Mouse Lisp

おもちゃ

## 方針

- 素朴。
- GCはbdwgcを使用。
- 健全なマクロ(syntax-rulesみたいな)も欲しいけど、古典的なマクロも欲しい。健全なマクロを古典的なマクロの上で実現するの難しいのだろうか？
  - できるらしい https://yhara.jp/RoadToSyntaxRules
- 様々な厳密さより書きやすさを優先する。
