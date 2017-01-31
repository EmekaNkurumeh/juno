https://raw.githubusercontent.com/rxi/lovedos/master/doc/api.md
# Juno API Reference

## [Callbacks](#callbacks-1)

## [Modules](#modules-1)
* [juno.graphics](#junographics)
* [juno.mouse](#junomouse)
* [juno.system](#junosystem)
* [juno.audio](#junoaudio)
* [juno.fs](#junofs)
* [juno.time](#junotime)
* [juno.bufferfx](#junobufferfx)
* [juno.debug](#junodebug)

## [Objects](#objects-1)
* [Data](#junodata)
* [Source](#junosource)
* [Buffer](#junobuffer)
* [Gif](#junogif)
* [Font](#junofont)

## [Config](#config-1)

## Callbacks
#### `juno.onLoad()`
#### `juno.onUpdate(dt)`
#### `juno.onDraw()`
#### `juno.onKeyDown(key)`
#### `juno.onKeyUp(key, char)`
#### `juno.onMouseMove(x,y)`
#### `juno.onMouseDown(x, y, button)`
#### `juno.onMouseUp(x, y, button)`
#### `juno.onQuit()`
#### `juno.onError(msg, stacktrace)`
#### `juno.getVersion()`
#### `juno._pcall(fn, ...)`
#### `juno._argv`

## juno.Data
#### `juno.Data.fromFile(filename)`
#### `juno.Data.fromString(string)`
#### `Data:getLength()`
#### `Data:toString()`
#### `Data:__gc()`

## juno.Source
#### `juno.Source.fromBlank()`
#### `juno.Source.fromData(data)`
#### `Source:setPan(pan)`
#### `Source:getLength()`
#### `Source:pause()`
#### `Source:getState()`
#### `Source:setDestination(dest)`
#### `Source:setRate(rate)`
#### `Source:stop()`
#### `Source:setGain(gain)`
#### `Source:play(reset)`
#### `Source:setLoop(loop)`
#### `Source:setCallback(callback)`
#### `Source:__gc()`

## juno.Buffer
#### `juno.Buffer.fromFile(filename)`
#### `juno.Buffer.fromString(string)`
#### `juno.Buffer.fromBlank(w, h)`
#### `Buffer:clone()`
#### `Buffer:getWidth()`
#### `Buffer:getHeight()`
#### `Buffer:getSize()`
#### `Buffer:setAlpha(alpha)`
#### `Buffer:setBlend(mode)`
#### `Buffer:setColor(r,g,b,a)`
#### `Buffer:setClip(x, y, w, h)`
#### `Buffer:reset()`
#### `Buffer:clear(r,g,b,a)`
#### `Buffer:getPixel(x, y)`
#### `Buffer:setPixel(x, y, r, g, b, a)`
#### `Buffer:copyPixels(src, x, y, sub, sx, sy)`
#### `Buffer:noise(seed, low, high, grey)`
#### `Buffer:floodFill(x, y, r, g, b, a)`
#### `Buffer:drawPixel(x, y, r, g, b, a)`
#### `Buffer:drawLine(x1, y1, x2, y2, r, g, b, a)`
#### `Buffer:drawRect(x, y, w, h, r, g, b, a)`
#### `Buffer:drawBox(x, y, w, h, r, g, b, a)`
#### `Buffer:drawCircle(x, y, radius, r, b, g, a)`
#### `Buffer:drawRing(x, y, radius, r, b, g, a)`
#### `Buffer:drawBuffer(src, x, y, sub, rot, sx, sy, ox, oy)`
#### `Buffer:draw(src, x, y, sub, rot, sx, sy, ox, oy)`
#### `Buffer:drawText(font, text, x, y, width)`
#### `Buffer:__gc()`

## juno.Gif
#### `juno.Gif.new(w, h, ncolors)`
#### `Gif:update(buf, delay)`
#### `Gif:close()`
#### `Gif:__gc()`

## juno.Font
#### `juno.Font.fromFile(filename, size)`
#### `juno.Font.fromString(string, size)`
#### `juno.Font.fromEmbedded(size)`
#### `Font:render(string)`
#### `Font:getWidth()`
#### `Font:getHeight()`
#### `Font:__gc()`

## juno.mouse
#### `juno.mouse.isDown(key)`
#### `juno.mouse.wasPressed(key)`
#### `juno.mouse.setVisible(visible)`
#### `juno.mouse.getPosition()`
#### `juno.mouse.setPosition(x, y)`
#### `juno.mouse.getX()`
#### `juno.mouse.getY()`

## juno.system
#### `juno.system.info(type)`
#### `juno.system.poll()`

## juno.audio
#### `juno.audio.master`

## juno.fs
#### `juno.fs.append(filename, data)`
#### `juno.fs.getSize(filename)`
#### `juno.fs.delete(filename)`
#### `juno.fs.isDir(filename)`
#### `juno.fs.unmount(path)`
#### `juno.fs.getModified(filename)`
#### `juno.fs.exists(filename)`
#### `juno.fs.mount(path)`
#### `juno.fs.setWritePath(path)`
#### `juno.fs.write(filename, data)`
#### `juno.fs.read(filename)`
#### `juno.fs.listDir(path)`
#### `juno.fs.makeDirs(path)`

## juno.time
#### `juno.time.getAverage()`
#### `juno.time.getNow()`
#### `juno.time.getTime()`
#### `juno.time.getDelta()`
#### `juno.time.sleep(seconds)`
#### `juno.time.step()`
#### `juno.time.getFps()`

## juno.graphics
#### `juno.graphics.setColor(r,g,b,a)`
#### `juno.graphics.drawCircle(x, y, radius, r, b, g, a)`
#### `juno.graphics.getPixel(x, y)`
#### `juno.graphics.drawBuffer(src, x, y, sub, rot, sx, sy, ox, oy)`
#### `juno.graphics.floodFill(x, y, r, g, b, a)`
#### `juno.graphics.copyPixels(src, x, y, sub, sx, sy)`
#### `juno.graphics.setFullscreen(fullscreen)`
#### `juno.graphics.getFullscreen()`
#### `juno.graphics.setBlend(mode)`
#### `juno.graphics.drawRing(x, y, radius, r, b, g, a)`
#### `juno.graphics.getWidth()`
#### `juno.graphics.setClearColor(r, g, b, a)`
#### `juno.graphics.getHeight()`
#### `juno.graphics.clear(r,g,b,a)`
#### `juno.graphics.reset()`
#### `juno.graphics.drawBox(x, y, w, h, r, g, b, a)`
#### `juno.graphics.clone()`
#### `juno.graphics.setAlpha(alpha)`
#### `juno.graphics.setPixel(x, y, r, g, b, a)`
#### `juno.graphics.noise(seed, low, high, grey)`
#### `juno.graphics.getSize()`
#### `juno.graphics.drawPixel(x, y, r, g, b, a)`
#### `juno.graphics.drawPixel(x, y, r, g, b, a)`
#### `juno.graphics.draw(src, x, y, sub, rot, sx, sy, ox, oy)`
#### `juno.graphics.drawRect(x, y, w, h, r, g, b, a)`
#### `juno.graphics.setClip(x, y, w, h)`
#### `juno.graphics.drawLine(x1, y1, x2, y2, r, g, b, a)`
#### `juno.graphics.drawText(font, text, x, y, width)`
#### `juno.graphics.__gc()`

## juno.bufferfx
#### `juno.bufferfx.desaturate(self, amount)`
#### `juno.bufferfx.dissolve(self,amount, s? investigate later)`
#### `juno.bufferfx.blur(self, src, rx, ry, )`
#### `juno.bufferfx.wave(self, src, ax, ay, sx, sy, ox, oy)`
#### `juno.bufferfx.mask(self, mask, channel)`
#### `juno.bufferfx.palette(self, palette)`
#### `juno.bufferfx.displace(self, src, map, cx, cy, sx, sy)`

## juno.debug
#### `juno.debug.setVisible(visible)`
#### `juno.debug.setFocused(focused)`
#### `juno.debug.getVisible()`
#### `juno.debug.getFocused()`
#### `juno.debug.removeIndicator(indicator)`
#### `juno.debug.addIndicator(fn, min, max)`
#### `juno.debug.clear()`

## juno.keyboard
#### `juno.keyboard.isDown(key,...)`
#### `juno.keyboard.wasPressed(key,...)`

## Config
#### `config.title`
#### `config.width`
#### `config.height`
#### `config.max_fps`
#### `config.samplerate`
#### `config.buffersize`
#### `config.fullscreen`
#### `config.resizable`
#### `config.borderless`
