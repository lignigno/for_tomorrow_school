<div align="center">
	<img src="header.gif" alt="DfD header"></img>
</div>

<br>
<br>
<br>

<div align="center">
	<h1>DirectMedia for Ducklings</h1>
</div>

<br>

# **INSTALL**

```
git clone https://github.com/lignigno/for_tomorrow_school.git && \
for_tomorrow_school/dfd/install.sh
```

<br>

# **INCLUDE**

```
go get github.com/lignigno/for_tomorrow_school/dfd/package@v0.0.1
```

```
import "github.com/lignigno/for_tomorrow_school/dfd/package"
```

<br>

# **EXAMPLE**

```
package main

import (
	"os"

	dfd "github.com/lignigno/for_tomorrow_school/dfd/package"
)

const (
	WIDTH  = 256
	HEIGHT = 256
)

func setPixel(r, g, b, a byte) uint32 {
	return uint32(r)<<24 | uint32(g)<<16 | uint32(b)<<8 | uint32(a)
}

func main() {
	win := dfd.CreateWindow("example", WIDTH, HEIGHT)

	for y := 0; y < win.Height; y++ {
		for x := 0; x < win.Width; x++ {
			win.Surface[y*win.Width+x] = setPixel(0, byte(x), byte(y), 255)
		}
	}

	event := dfd.EventContainer{}
	for {
		for dfd.PollEvent(&event) {
			if event.Type == dfd.EVENT_QUIT {
				os.Exit(0)
			}
		}

		win.Update()
	}
}
```