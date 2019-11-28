package lextoken

type LexToken struct {
	Row int
	Column int
	Type TokenT
	Value string
}

