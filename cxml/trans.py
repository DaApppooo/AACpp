from sys import argv, stderr
import xml.etree.ElementTree as ET
from typing import Self, Callable
import os

with open(argv[1]) as f:
  code = f.read()

ATTRIBUTES = (
  'id',
  'layout',
    'sizing',
      'width',
      'height',
    'padding',
      'left',
      'right',
      'top',
      'bottom',
    'childGap',
    'childAlignment',
      'x',
      'y',
    'layoutDirection',
  'backgroundColor',
  'cornderRadius',
  'image',
    'imageData',
    'sourceDimensions',
  'floating',
  # ...
  'custom',
  'scroll',
  'border'
)

class FieldBuilder:
  @staticmethod
  def use(attrs: dict[str,str]) -> Self:
    f = FieldBuilder()
    for k,v in attrs.items():
      if '.' in k:
        base, particle = k.split('.',1)
        if base in f._dct:
          continue
        f._dct[base] = FieldBuilder.use({
          k.split('.',1)[1]:attrs[k]
          for k in attrs
          if k.startswith(base+'.')
        })
      else:
        f._dct[k] = v
    return f
  def __init__(self):
    self._dct = {}
  def __setattr__(self, name, value):
    if name.startswith("_"):
      object.__setattr__(self, name, value)
    else:
      self._dct[name] = value.strip()
  def __getattribute__(self, name):
    if name.startswith("_") or name == 'func' or name == 'alias':
      return object.__getattribute__(self, name)
    elif name in self._dct:
      return self._dct[name] + ','
    else:
      return ""
  def func(self, key, fmt: str | Callable[str, [str]]):
    if '.' in key:
      base, particle = key.split('.',1)
      if base in self._dct:
        assert isinstance(self._dct[base], FieldBuilder), f"{base} should be a field builder"
        self._dct[base].func(particle, fmt)
    elif key in self._dct and not self._dct[key].startswith('?'):
      if callable(fmt):
        self._dct[key] = fmt(self._dct[key])
      else:
        self._dct[key] = fmt.format(self._dct[key])
  def alias(self, alias: str, key: str):
    if alias in self._dct:
      if '.' in key:
        parts = key.split('.')
        orig = self
        for p in parts[:-1]:
          if p not in self._dct:
            self._dct[p] = FieldBuilder()
          self = self._dct[p]
        self._dct[parts[-1]] = orig._dct[alias]
        del orig._dct[alias]
      else:
        self._dct[key] = self._dct[alias]
        del self._dct[alias]
  def __call__(self):
    def fmt(v):
      if isinstance(v, str):
        if v[0] == '?':
          return v[1:]
        return v
      assert isinstance(v, FieldBuilder)
      return v()
    return (
      '{'
      + ','.join(
        f".{k}={fmt(v)}"
        for k,v in sorted(self._dct.items(), key=lambda x: ATTRIBUTES.index(x[0]))
      )
      + '}'
    )

class ClayBox:
  def __init__(self, default_attrs):
    self.default_attrs = default_attrs
  def __call__(self, attr: dict[str, str], text: str):
    field = FieldBuilder.use(
      attr | self.default_attrs
    )
    field.alias('w', 'layout.sizing.width')
    field.alias('h', 'layout.sizing.height')
    field.alias('pad', 'layout.padding')
    field.alias('child-align', 'layout.childAlignment')
    field.alias('cg', 'layout.childGap')
    field.alias('bg', 'backgroundColor')
    field.alias('r', 'cornerRadius')
    field.alias('z', 'floating.zIndex')

    field.func('id', "CLAY_ID(\"{}\")")
    CHILD_ALIGNEMENTS = {
      'center': 'CLAY_ALIGN_{}_CENTER',
      'top': 'CLAY_ALIGN_{}_TOP',
      'bottom': 'CLAY_ALIGN_{}_BOTTOM',
      'right': 'CLAY_ALIGN_{}_LEFT',
      'left': 'CLAY_ALIGN_{}_RIGHT'
    }
    field.func('layout.childAlignment.x', lambda x: CHILD_ALIGNEMENTS[x.lower()].format('X'))
    field.func('layout.childAlignment.y', lambda x: CHILD_ALIGNEMENTS[x.lower()].format('Y'))
    SCROLL = {
      'v': '{.vertical=true}',
      'h': '{.horizontal=true}',
      'vertical': '{.vertical=true}',
      'horizontal': '{.horizontal=true}',
      'vh': '{.horizontal=true,.vertical=true}',
      'hv': '{.horizontal=true,.vertical=true}',
      'vertical-horizontal': '{.horizontal=true,.vertical=true}',
      'horizontal-vertical': '{.horizontal=true,.vertical=true}',
    }
    field.func('scroll', lambda x: SCROLL[x.lower()])
    def SIZING(expr):
      if expr.lower() == 'grow':
        return "CLAY_SIZING_GROW(0)"
      elif ',' in expr:
        return "CLAY_SIZING_FIT({},{})".format(*expr.split(',',1))
      else:
        return f"CLAY_SIZING_FIXED({expr})"
    field.func('layout.sizing.width', SIZING)
    field.func('layout.sizing.height', SIZING)
    def PADDING(expr):
      args = expr.split(',')
      if len(args) == 1:
        return f'{{{args[0]},{args[0]},{args[0]},{args[0]}}}'
      elif len(args) == 2:
        return f'{{{args[0]},{args[0]},{args[1]},{args[1]}}}'
      elif len(args) == 4:
        return f'{{{args[0]},{args[1]},{args[2]},{args[3]}}}'
      else:
        assert False, "Padding syntax must be one of: lrtb | x,y | left,right,top,bottom"
    field.func('layout.padding', PADDING)
        

    # Indexed ID (first transform, then mixin with basic ID)
    field.func('iid', lambda x: 'CLAY_IDI("{}", {})'.format(*x.split('=')))
    field.alias('iid', 'id')
    return f"CLAY({field()}){{{text}}}"

def clay_text(attrs, text):
  assert "config" in attrs or "textConfig" in attrs
  if "config" in attrs:
    config = attrs["config"]
  else:
    config = attrs["textConfig"]
  return f"CLAY_TEXT({text}, {config});"

tags = {
  "body": ClayBox({'layout.sizing.width': 'GROW', 'layout.sizing.height': 'GROW'}),
  "column": ClayBox({'layout.layoutDirection':'CLAY_TOP_TO_BOTTOM'}),  
  "row": ClayBox({'layout.layoutDirection':'CLAY_LEFT_TO_RIGHT'}),
  "box": ClayBox({}),
  "margin": ClayBox({'backgroundColor': '{0,0,0,0}'}),
  "img": ClayBox({'backgroundColor': '{255,255,255,255}'}),
  "text": clay_text
}

def parse(elem):
  OUT = ""
  for i in elem:
    assert isinstance(i, ET.Element)
    OUT += parse(i)
    OUT += i.tail and i.tail.strip().replace("\n",'') or ""
  OUT = tags[elem.tag](elem.attrib, (elem.text or '') + OUT)
  return OUT

def name(path):
  return path.rsplit('/', 1)[1].rsplit('.',1)[0]

o  = "#include \"ui.hpp\"\n"
o += f"void layout_{name(argv[1])}(Clay_RenderCommandArray& rc) {{\n"
o += "Clay_BeginLayout();"
o += parse(ET.fromstring(code))
o += "rc = Clay_EndLayout();}"
proc = os.popen("g++ -fPIC -o cxml/ui.so -shared -xc++ -", 'w')
proc.write(o)
proc.close()
