package cursor

import (
	"bufio"
	"io"
)


type Cursor struct {
	Row int
	Column int
	r int
	c int
	reader *bufio.Reader
	line string
	LastChar rune
}

func(c *Cursor) Next() (rune,bool){

	if c.c >= len(c.line) {
		c.r ++
		if l, _, err := c.reader.ReadLine(); err != nil {
			if err == io.EOF {
				return rune(-1), false
			} else {
				panic(err)
			}
		} else {
			c.line = string(l)
			c.c = 0
			if c.r > 1 {
				c.Column++
				c.LastChar = '\n'
				return c.LastChar,true
			}
		}
	}

	c.Row = c.r
	c.getNextChar()
	c.Column = c.c

	return c.LastChar,true
}

func(c *Cursor) getNextChar() {
	if c.c < len(c.line) {
		c.LastChar = rune(c.line[c.c])
		c.c ++
		return
	}

	c.LastChar = '\n'
}

func(c *Cursor) Init(reader io.Reader){
	c.reader = bufio.NewReader(reader)
}

func (c *Cursor)Back(){
	if c.LastChar != '\n'{
		c.c--
		c.Column--
	} else {
		c.c = c.Column
		c.r = c.Row
	}
}

