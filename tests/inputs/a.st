add = (lam x: N. lam y: N. rec x (lam k: N. (lam r: N. (S r))) y)

intermediate = add 3

main = intermediate 1
