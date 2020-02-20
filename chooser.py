# -*- python -*-

# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. However, to
# facilitate maintenance we ask that before distributing modified
# versions of this software, you first contact the authors at
# oof_manager@nist.gov. 

from gi.repository import GObject
from gi.repository import Gtk
from gi.repository import Gdk
import types

# The ChooserWidget creates a pull-down menu containing the given list
# of names.  The currently selected name is shown when the menu isn't
# being pulled down.

# Calls the callback, if specified, with the gtk-selecting object
# and the name.  Backward compatible with other callbacks that way.
# Also has an "update_callback" which gets called, with the new
# current selection, when the list of things is updated, with one
# exception -- it is *not* called at __init__-time.

# The ChooserListWidgets in this file have both a list of names that
# they display, and an optional list of objects corresponding to those
# names.  When asked for their 'value', they return the object, not
# the name.  For historical reasons, the ChooserWidget does not do
# this, although it could be made TODO so easily.

## TODO: The callback functions for different kinds of ChooserWidgets
## take different arguments.  Make them all the same.  The
## ChooserWidget callback should not include the gtk object in its
## args.  It's never used.

## The ChooserWidget is a Gtk.Label with a pop-up menu, instead of a
## Gtk.ComboBox with a Gtk.ListStore and Gtk.TreeView.  The menu items
## can have tooltips.  The cells in a TreeView can't have tooltips.
## The ChooserComboWidget can stay as a Gtk.ComboBox because it lists
## user-defined entries, which don't need tooltips.

## TODO: separator_func needs to be implemented differently. Use
## GtkSeparatorMenuItem
        
class ChooserWidget(object):
    def __init__(self, namelist, callback=None, callbackargs=(),
                 update_callback=None, update_callback_args=(),
                 helpdict={}, name=None, separator_func=None):
        self.separator_func = separator_func
        self.current_string = None
        self.current_item = None # only exists while the menu is visible
        self.callback = callback
        self.callbackargs = callbackargs
        self.helpdict = helpdict
        self.signal = None #
        self.namelist = namelist[:]

        self.gtk = Gtk.EventBox()
        frame = Gtk.Frame()
        self.gtk.add(frame)
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        frame.add(hbox)
        if namelist:
            name0 = namelist[0]
        else:
            name0 = ""
        self.label = Gtk.Label(name0, halign=Gtk.Align.START,
                               hexpand=True, # TODO: True?
                               margin_start=2, margin_end=4)
        hbox.pack_start(self.label, expand=True, fill=True, padding=2)
        image = Gtk.Image.new_from_icon_name('pan-down-symbolic',
                                             Gtk.IconSize.BUTTON)
        hbox.pack_start(image, expand=False, fill=False, padding=2)
        self.gtk.connect("button-press-event", self.buttonpressCB)

    def show(self):
        self.gtk.show_all()
    def hide(self):
        self.gtk.hide()
    def destroy(self):
        self.gtk.destroy()
    def buttonpressCB(self, gtkobj, event):
        popupMenu = Gtk.Menu()
        popupMenu.set_size_request(self.gtk.get_allocated_width(), -1)
        newCurrentItem = None
        for name in self.namelist:
            if self.separator_func and self.separator_func(name):
                menuitem = Gtk.SeparatorMenuItem()
            else:
                menuitem = Gtk.MenuItem(name)
                menuitem.connect('activate', self.activateCB, name)
                menuitem.connect('enter-notify-event', self.enterItemCB)
                helpstr = self.helpdict.get(name, None)
                if helpstr:
                    menuitem.set_tooltip_text(helpstr)
                if name == self.current_string:
                    newCurrentItem = menuitem
                    # menuitem.connect('select', self.selectCB, popupMenu)
            popupMenu.append(menuitem)
        if newCurrentItem:
            self.current_item = newCurrentItem
            self.current_item.select()
        else:
            self.current_item = None
        popupMenu.show_all()
        popupMenu.popup_at_widget(self.label, Gdk.Gravity.SOUTH_WEST,
                                  Gdk.Gravity.NORTH_WEST, event)

        return False
    def activateCB(self, menuitem, name):
        self.label.set_text(name)
        self.current_string = name
        # self.current_item = menuitem
        if self.callback:
            self.callback(*(name,) + self.callbackargs)
    # def selectCB(self, menuitem, menu):
    #     print "selectCB"
    #     return False
    def enterItemCB(self, menuitem, event):
        if self.current_item is not None:
            self.current_item.deselect()
            self.current_item = None

    def update(self, namelist, helpdict={}):
        self.namelist = namelist[:]
        self.helpdict= helpdict
        if self.current_string not in namelist:
            self.current_string = None
        if self.update_callback:
            self.update_callback(*(self.current_string,) +
                                 self.update_callback_args)
    def set_state(self, arg):
        # arg is either an integer position in namelist or a string in
        # namelist.
        if type(arg) == types.IntType:
            newstr = self.namelist[arg]
        elif type(arg) == types.StringType:
            if arg in self.namelist:
                newstr = arg
            else:
                newstr = self.namelist[0]
        else:
            raise ValueError("Invalid value: " + `arg`)
        if newstr != self.current_string:
            self.label.set_text(self.current_string)
            if self.update_callback:
                self.update_callback(*(self.current_string,) +
                                     self.update_callback_args)
            
    def get_value(self):
        return self.current_string
    def nChoices(self):
        return len(self.namelist)
    def choices(self):
        return self.namelist
            
                

class OLDChooserWidget:
    def __init__(self, namelist, callback=None, callbackargs=(),
                 update_callback=None, update_callback_args=(), helpdict={},
                 name=None, separator_func=None):
        #assert name is not None
        
        # If this is used as a base class for another widget, self.gtk
        # will be redefined.  So if a ChooserWidget function needs to
        # refer to the ComboBox gtk widget, it must use
        # self.combobox instead of self.gtk.
        liststore = Gtk.ListStore(GObject.TYPE_STRING)
        self.combobox = Gtk.ComboBox.new_with_model(liststore)
        cell = Gtk.CellRendererText()
        self.combobox.pack_start(cell, True)
        self.combobox.set_cell_data_func(cell, self.cell_layout_data_func)
        # self.combobox.set_property("has-tooltip", True)
        # self.combobox.connect('query-tooltip', self.tooltipCB)

        # If separator_func is provided, it must be a function that
        # takes a gtk.TreeModel and gtk.TreeIter as arguments, and
        # return True if the row given by model[iter] is to be
        # represented by a separator in the TreeView.
        if separator_func:
            self.combobox.set_row_separator_func(separator_func)
        self.gtk = self.combobox
        self.current_string = None
        self.callback = callback
        self.callbackargs = callbackargs
        self.helpdict = {}
        # self.tipmap = {}                # see cell_layout_data_func()
        self.signal = self.combobox.connect('changed',
                                            self.changedCB)
        # make sure that the update_callback isn't installed until
        # after the widget is initialized.
        self.update_callback = None
        self.update_callback_args = ()
        self.update(namelist, helpdict)
        self.update_callback = update_callback
        self.update_callback_args = update_callback_args

    # def tooltipCB(self, combo, x, y, keyboard_mode, tooltip, *data):
    #     tooltip.set_text("(%d, %d)" %  (x, y))
    #     return True

    count = 0
    def cell_layout_data_func(self, cell_view, cell_renderer, model, iter):
        # print "ChooserWidget.cell_layout_data_func", self.count
        # self.count += 1
        idx = model.get_path(iter)[0]
        item_text = model.get_value(iter, 0)
        cell_renderer.set_property('text', item_text)

        # cell_view is the Gtk.ComboBox
        cell_view.set_tooltip_text("Hello " + `self.count`)
            

        ## TODO: This next section should be done differently.  See
        ## https://developer.gnome.org/gtk3/stable/GtkTooltip.html
        # try:
        #     tip_text = self.helpdict[item_text]
        # except KeyError:
        #     pass
        # else:
        #     # When navigating using the arrow keys, cell_view is
        #     # sometimes a TreeViewColumn instead of a CellView.
        #     # TreeViewColumns aren't widgets and don't have
        #     # get_parent(), so we just ignore them.
        #     try:
        #         cv_parent = cell_view.get_parent()
        #     except AttributeError:
        #         pass
        #     else:
        #         if isinstance(cv_parent, Gtk.MenuItem) \
        #                and (cv_parent not in self.tipmap or
        #                     self.tipmap[cv_parent] != tip_text):
        #             self.tipmap[cv_parent] = tip_text
        #             cv_parent.set_tooltip_text(tip_text)

    def show(self):
        self.gtk.show_all()

    def hide(self):
        self.gtk.hide()

    def destroy(self):
        self.gtk.destroy()

    def suppress_signals(self):
        self.combobox.handler_block(self.signal)
        # self.signal.block()
    def allow_signals(self):
        self.combobox.handler_unblock(self.signal)
        # self.signal.unblock()

    def changedCB(self, combobox):
        model = combobox.get_model()
        index = combobox.get_active()
        self.current_string = model[index][0]
        self.set_tooltip()
        if self.callback:
            self.callback(* (combobox, self.current_string)+self.callbackargs)

    def update(self, namelist, helpdict={}):
        
        # Replace the widget's list of names with the given list.  The
        # original value of self.current_string will be restored if it
        # still exists in the updated namelist.
        try:
            current_index = namelist.index(self.current_string)
        except ValueError:
            if len(namelist) > 0:
                current_index = 0
            else:
                current_index = -1      # no selection
        self.helpdict = helpdict
        self.suppress_signals()
        liststore = self.combobox.get_model()
        liststore.clear()
        for name in namelist:
            liststore.append([name])
        self.combobox.set_active(current_index)
        if current_index >= 0:
            self.current_string = liststore[current_index][0]
        else:
            self.current_string = None
        self.allow_signals()
        self.set_tooltip()
        if self.update_callback:
            self.update_callback(*(self.gtk, self.current_string)+
                                 self.update_callback_args)
        self.combobox.set_sensitive(len(namelist) > 0)

        # Make the widget wide enough to fit the longest name it
        # might display, plus some extra space for the arrow
        # decoration.
        if namelist:
            maxlen = max([len(name) for name in namelist])
            self.combobox.set_size_request(30, -1)
            # self.combobox.set_size_request(30+maxlen*guitop.top().charsize, -1)
                                  
    def set_tooltip(self):
        self.combobox.set_tooltip_text(
            self.helpdict.get(self.current_string, None))

    def set_state(self, arg):
        self.suppress_signals()
        model = self.combobox.get_model()
        if type(arg) == types.IntType:
            self.combobox.set_active(arg)
            self.current_string = model[arg][0]
        elif type(arg) == types.StringType:
            names = [row[0] for row in model]
            try:
                index = names.index(arg)
            except ValueError:
                self.combobox.set_active(0)
                self.current_string = names[0]
            else:
                self.combobox.set_active(index)
                self.current_string = arg
        self.allow_signals()
        self.set_tooltip()
        if self.update_callback:
            self.update_callback(*(self.gtk, self.current_string)+
                                 self.update_callback_args)
            
    def get_value(self):
        return self.current_string
    def nChoices(self):
        return len(self.combobox.get_model())
    def choices(self):
        model = self.combobox.get_model()
        return [x[0] for x in iter(model)]

## ### #### ##### ###### ####### ######## ####### ###### ##### #### ### ##

# The NewChooserWidget has a pulldown menu and a button.  The default
# text on the button is "New...".  The intent is that the callback for
# the button will create a new object to be listed in the pulldown
# menu. The button callback is responsible for updating the menu (with
# ChooserWidget.update) and selecting the new entry (with
# ChooserWidget.set_state).

## NOT THE SAME AS THE NEWChooserWidget, above, that will replace the
## ChooserWidget.

class NewChooserWidget(ChooserWidget):
    def __init__(self, namelist, callback=None, callbackargs=(),
                 update_callback=None, update_callback_args=(),
                 button_callback=None, button_callback_args=(),
                 buttontext="New...",
                 separator_func=None):
        ChooserWidget.__init__(self, namelist,
                               callback=callback, callbackargs=callbackargs,
                               update_callback=update_callback,
                               update_callback_args=update_callback_args,
                               helpdict={}, separator_func=separator_func)

        self.button_callback = button_callback
        self.button_callback_args = button_callback_args
        
        # Wrap the ChooserWidget's gtk in a GtkHBox and make it become self.gtk
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
        hbox.pack_start(self.gtk, expand=True, fill=True, padding=2)
        self.gtk = hbox

        self.newbutton = Gtk.Button(buttontext)
        hbox.pack_start(self.newbutton, expand=0, fill=0, padding=2)
        self.newbutton.connect('clicked', self.newbuttonCB)
    def newbuttonCB(self, button):
        if self.button_callback:
            self.button_callback(*self.button_callback_args)
    def set_button_sensitivity(self, sensitivity):
        self.newbutton.set_sensitive(sensitivity)
            

## ### #### ##### ###### ####### ######## ####### ###### ##### #### ### ##    

# Like a ChooserWidget, but makes a list instead of a pull-down menu.
# Locally stateful.  Callback gets called when selection state
# changes, with the newly-selected string or "None".  Takes both a
# list of objects, and a list of display strings for those objects.
# If comparison of two objects is nontrivial and not implemented by
# the objects' __eq__ function, the 'comparator' arg should be
# provided.  It should be a function of two objects that returns 1 if
# they are equal.

# 'callback' is called when an item is selected in the list.
# 'dbcallback' is called when an item is double-clicked, or 'return'
# is pressed when an item is selected and the list has focus.

## TODO MAYBE: Use helpdict to put tooltips on the list items.  This
## probably isn't important.  Most, maybe all, ChooserListWidgets
## display user-created objects, and don't have helpdicts.

class ChooserListWidgetBase:
    def __init__(self, objlist=None, displaylist=[], callback=None,
                 dbcallback=None, autoselect=True, helpdict={},
                 comparator=None, markup=False,
                 name=None, separator_func=None):
        self.liststore = Gtk.ListStore(GObject.TYPE_STRING,
                                       GObject.TYPE_PYOBJECT)
        self.treeview = Gtk.TreeView(self.liststore)
        self.gtk = self.treeview
        self.treeview.set_property("headers-visible", 0)
        cell = Gtk.CellRendererText()
        if markup:
            # Expect to find pango markup in displaylist, which is
            # stored in column 0 of the ListStore.
            self.tvcol = Gtk.TreeViewColumn("", cell, markup=0)
        else:
            self.tvcol = Gtk.TreeViewColumn("", cell)
        self.treeview.append_column(self.tvcol)
        self.tvcol.add_attribute(cell, 'text', 0)
        self.autoselect = autoselect
        self.callback = callback or (lambda x, interactive=False: None)
        self.dbcallback = dbcallback or (lambda x: None)
        self.comparator = comparator or (lambda x, y: x == y)
        self.activatesignal = self.treeview.connect('row-activated',
                                               self.rowactivatedCB)

        # If separator_func is provided, it must be a function that
        # takes a gtk.TreeModel and gtk.TreeIter as arguments, and
        # return True if the row given by model[iter] is to be
        # represented by a separator in the TreeView.
        if separator_func:
            self.treeview.set_row_separator_func(separator_func)

        selection = self.treeview.get_selection()
        self.selectsignal = selection.connect('changed',
                                             self.selectionchangedCB)
        self.update(objlist or [], displaylist, helpdict=helpdict)

    def grab_focus(self):
        self.treeview.grab_focus()

    def suppress_signals(self):
        self.treeview.handler_block(self.activatesignal)
        self.treeview.get_selection().handler_block(self.selectsignal)
        # self.activatesignal.block()
        # self.selectsignal.block()
    def allow_signals(self):
        self.treeview.handler_unblock(self.activatesignal)
        self.treeview.get_selection().handler_unblock(self.selectsignal)
        # self.activatesignal.unblock()
        # self.selectsignal.unblock()

    def find_obj_index(self, obj):
        if obj is not None:
            objlist = [r[1] for r in self.liststore]
            for which in range(len(objlist)):
                if self.comparator(obj, objlist[which]):
                    return which
        raise ValueError

    def rowactivatedCB(self, treeview, path, col):
        self.dbcallback(self.get_value())
    def selectionchangedCB(self, treeselection):
        self.callback(self.get_value(), interactive=True)

    def show(self):
        self.gtk.show_all()
    def hide(self):
        self.gtk.hide()
    def destroy(self):
        self.gtk.destroy()

class ChooserListWidget(ChooserListWidgetBase):
    # Get the index of the current selection.
    def get_index(self):
        treeselection = self.treeview.get_selection() # gtk.TreeSelection obj
        (model, iter) = treeselection.get_selected() #gtk.ListStore,gtk.TreeIter
        if iter is not None:
            return model.get_path(iter)[0]  # integer!
    def has_selection(self):
        selection = self.treeview.get_selection()
        model, iter = selection.get_selected()
        return iter is not None
    def get_value(self):
        selection = self.treeview.get_selection()
        model, iter = selection.get_selected()
        if iter is not None:
            return model[iter][1]
    def set_selection(self, obj):
        self.suppress_signals()
        treeselection = self.treeview.get_selection()
        try:
            which = self.find_obj_index(obj)
        except ValueError:
            treeselection.unselect_all()
        else:
            treeselection.select_path(which)
            self.treeview.scroll_to_cell(which)
        self.allow_signals()

    def scroll_to_line(self, lineno):
        self.treeview.scroll_to_cell(lineno)
        
    # Replace the contents, preserving the selection state, if
    # possible.
    def update(self, objlist, displaylist=[], helpdict={}):
        self.suppress_signals()
        old_obj = self.get_value()
        self.liststore.clear()
        for obj, dispname in map(None, objlist, displaylist):
            if dispname is not None:
                self.liststore.append([dispname, obj])
            else:
                self.liststore.append([obj, obj])
        try:
            index = self.find_obj_index(old_obj)
        except ValueError:
            if self.autoselect and len(objlist) == 1:
                # select the only object in the list
                treeselection = self.treeview.get_selection()
                treeselection.select_path(0)
            # New value differs from old value, so callback must be invoked.
            self.callback(self.get_value(), interactive=False)
        else:                           # reselect current_obj
            treeselection = self.treeview.get_selection()
            treeselection.select_path(index)
        self.allow_signals()

# List widget that allows multiple selections

class MultiListWidget(ChooserListWidgetBase):
    def __init__(self, objlist, displaylist=[], callback=None,
                 dbcallback=None, autoselect=True, helpdict={},
                 comparator=None, name=None, separator_func=None,
                 markup=False):
        ChooserListWidgetBase.__init__(self, objlist, displaylist, callback,
                                       dbcallback, autoselect, helpdict,
                                       comparator=comparator, name=name,
                                       separator_func=separator_func,
                                       markup=markup)
        selection = self.treeview.get_selection()
        selection.set_mode(Gtk.SelectionMode.MULTIPLE)
    def get_value(self):
        selection = self.treeview.get_selection()
        model, rows = selection.get_selected_rows()
        return [model[r][1] for r in rows]
    def has_selection(self):
        selection = self.treeview.get_selection()
        model, rows = selection.get_selected_rows()
        return len(rows) > 0
    def update(self, objlist, displaylist=[], helpdict={}):
        self.suppress_signals()
        old_objs = self.get_value()
        self.liststore.clear()
        for obj, dispname in map(None, objlist, displaylist):
            if dispname is not None:
                self.liststore.append([dispname, obj])
            else:
                self.liststore.append([obj, obj])
        treeselection = self.treeview.get_selection()
        for obj in old_objs:
            try:
                index = self.find_obj_index(obj)
            except ValueError:
                pass
            else:
                treeselection.select_path(index)
                
        self.allow_signals()
    def set_selection(self, selectedobjs):
        # does not unselect anything, or emit signals
        self.suppress_signals()
        objlist = [r[1] for r in self.liststore]
        treeselection = self.treeview.get_selection()
        if selectedobjs:
            for obj in selectedobjs:
                try:
                    index = self.find_obj_index(obj)
                except ValueError:
                    pass
                else:
                    treeselection.select_path(index)
        self.allow_signals()
        self.callback(self.get_value(), interactive=False)

    def clear(self):
        self.suppress_signals()
        selection = self.treeview.get_selection()
        selection.unselect_all()
        self.allow_signals()

##################################################

class ChooserComboWidget:
    def __init__(self, namelist, callback=None, name=None):
        # If a callback is provided, it's called a *lot* of times.
        # It's called for every keystroke in the entry part of the
        # widget and every time a selection is made in the list part
        # of the widget.
        liststore = Gtk.ListStore(GObject.TYPE_STRING)
        self.combobox = Gtk.ComboBox.new_with_model_and_entry(liststore)
        self.combobox.set_entry_text_column(0)
        self.gtk = self.combobox
        self.namelist = []
        self.current_string = None
        self.update(namelist)
        self.signal = self.combobox.connect('changed',
                                        self.changedCB)
        self.callback = callback

    def show(self):
        self.gtk.show_all()

    def destroy(self):
        self.gtk.destroy()

    def suppress_signals(self):
        self.combobox.handler_block(self.signal)
        # self.signal.block()
    def allow_signals(self):
        self.combobox.handler_unblock(self.signal)
        # self.signal.unblock()

    def update(self, namelist):
        current_string = self.combobox.get_child().get_text()
        try:
            current_index = namelist.index(current_string)
        except ValueError:
            if len(namelist) > 0:
                current_index = 0
                current_string = namelist[0]
            else:
                current_index = -1
        liststore = self.combobox.get_model()
        liststore.clear()
        for name in namelist:
            liststore.append([name])
        self.combobox.get_child().set_text(current_string)
        self.combobox.set_active(current_index)

    def changedCB(self, combobox):
        self.callback(self)

    def get_value(self):
        val = self.combobox.get_child().get_text()
        return val

    def set_state(self, arg):
        if type(arg) == types.IntType:
            liststore = self.combobox.get_model()
            self.combobox.get_child().set_text(liststore[arg][0])
        elif type(arg) == types.StringType:
            self.combobox.get_child().set_text(arg)
        self.combobox.get_child().set_position(-1)


        
## ### #### ##### ###### ####### ######## ####### ###### ##### #### ### ##
## ### #### ##### ###### ####### ######## ####### ###### ##### #### ### ##

# Variants of the above widgets...

class FramedChooserListWidget(ChooserListWidget):
    def __init__(self, objlist=None, displaylist=[],
                 callback=None, dbcallback=None, autoselect=True,
                 comparator=None, name=None):
        ChooserListWidget.__init__(self,
                                   objlist=objlist,
                                   displaylist=displaylist,
                                   callback=callback,
                                   dbcallback=dbcallback,
                                   autoselect=autoselect,
                                   comparator=comparator,
                                   name=name)
        self.gtk = Gtk.Frame()
        self.gtk.set_shadow_type(Gtk.ShadowType.IN)
        self.gtk.add(self.treeview)

class ScrolledChooserListWidget(ChooserListWidget):
    def __init__(self, objlist=None, displaylist=[], callback=None,
                 dbcallback=None, autoselect=True, comparator=None, name=None,
                 separator_func=None, markup=False):
        ChooserListWidget.__init__(self,
                                   objlist=objlist,
                                   displaylist=displaylist,
                                   callback=callback,
                                   dbcallback=dbcallback,
                                   autoselect=autoselect,
                                   comparator=comparator,
                                   name=name,
                                   separator_func=separator_func,
                                   markup=markup)
        self.gtk = Gtk.ScrolledWindow()
        self.gtk.set_shadow_type(Gtk.ShadowType.IN)
        self.gtk.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        self.gtk.add(self.treeview)


class ScrolledMultiListWidget(MultiListWidget):
    def __init__(self, objlist=None, displaylist=[], callback=None, name=None,
                 separator_func=None):
        MultiListWidget.__init__(self, objlist, displaylist, callback,
                                 name=name, separator_func=separator_func)
        mlist = self.gtk
        self.gtk = Gtk.ScrolledWindow()
        self.gtk.set_shadow_type(Gtk.ShadowType.IN)
        self.gtk.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        self.gtk.add(mlist)
