1,$ {
  s/, \!.+//
  s/#[0-9]+/#0/
  /\!.+/d
  /;.+/d
  /declare void @Simple().+/d
  /target datalayout.+/d
  /target triple.+/d
  /attributes #.+/d
}

