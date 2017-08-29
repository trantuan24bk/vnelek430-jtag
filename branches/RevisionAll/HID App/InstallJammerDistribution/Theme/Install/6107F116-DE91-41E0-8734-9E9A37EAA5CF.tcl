proc CreateWindow.6107F116-DE91-41E0-8734-9E9A37EAA5CF {wizard id} {
    set base [$wizard widget get $id]

    grid rowconfigure    $base 1 -weight 1
    grid columnconfigure $base 1 -weight 1

    label $base.image -borderwidth 0 -background white
    grid  $base.image -row 0 -column 0 -rowspan 2 -sticky nw
    $id widget set Image -type image -widget $base.image

    Label $base.title -height 3 -bg white -font TkCaptionFont  -autowrap 1 -anchor nw -justify left
    grid $base.title -row 0 -column 1 -sticky ew -padx 20 -pady [list 20 10]
    $id widget set Caption -type text -widget $base.title

    Label $base.message -bg white -autowrap 1 -anchor nw -justify left
    grid  $base.message -row 1 -column 1 -sticky news -padx 20
    $id widget set Message -type text -widget $base.message

    Separator $base.sep -orient horizontal
    grid $base.sep -row 2 -column 0 -columnspan 2 -sticky ew
}

