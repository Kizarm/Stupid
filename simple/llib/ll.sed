1,$ {
  s/, \!.+//
  s/#[0-9]+/#0/
  /\!.+/d
  /;.+/d
  /declare void @Simple\(\).+/d
  /@GStation = external constant.+/d
  /target datalayout.+/d
  /target triple.+/d
  /attributes #.+/d
}

