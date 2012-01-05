import wx                    # needed for display stuff
from omniORB import CORBA    # use this for the CORBA orb stuff 
                             # (pushing packets)
import sys                   # for system commands (e.g., argv and argc stuff)
import CosNaming                          # narrowing naming context stuff
from ossie.cf import CF, CF__POA                        # core framework stuff
from ossie.standardinterfaces import standardInterfaces__POA
from wx.lib.anchors import LayoutAnchors  # used by splitter window
from time import localtime, strftime
import copy

#generate wx ids for my wx controls
[wxID_MAINFRAME, wxID_SPLITTERWINDOW1, wxID_WRITEPACKETBTN, 
wxID_WRITEBUFFERBTN, wxID_IFILENAMEEDITOR, wxID_QFILENAMEEDITOR, 
wxID_IFILESTATICTEXT,wxID_QFILESTATICTEXT] = [wx.NewId() for _init_ctrls 
                                                             in range(8)]


class MainFrame(wx.Frame):
    def __init__(self, orb, parent=None, id=-1, pos=wx.DefaultPosition, 
                 title="OSSIE Talk"): 

        self.orb_ref = orb

        self._init_ctrls(parent)

        self.I_data = []
        self.Q_data = []
         
        self.parent = parent
        self.my_local_controls = None
        
        # Now Create the menu bar and items
        self.mainmenu = wx.MenuBar()

        menu = wx.Menu()
        #menu.Append(210,'&Clear Text History','Erase the dirty talk...')
        menu.Append(205, 'E&xit', 'Enough of this already!')
        self.Bind(wx.EVT_MENU, self.OnFileExit, id=205)
        #self.Bind(wx.EVT_MENU,self.OnClearTextHistory,id=210)
        self.mainmenu.Append(menu, '&File')
        
        menu = wx.Menu()
        menu.Append(300, '&About', 'About this thing...')
        self.Bind(wx.EVT_MENU, self.OnHelpAbout, id=300)
        self.mainmenu.Append(menu, '&Help')

        self.SetMenuBar(self.mainmenu)

        # Bind the close event so we can disconnect the ports
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        self.Show(True)
        


    def _init_ctrls(self, prnt):
        frame_size = wx.Size(180, 80)
        talk_btn_size = wx.Size(175,50)
        #rx_text_box_size = (400, 130)
        #text_edit_size = wx.Size(400,30)
        #text_btn_size = wx.Size(145,50)

                     
        wx.Frame.__init__(self, id=-1, name='', parent=prnt,
                          pos=wx.Point(1, 570), size=frame_size,
                          style=wx.DEFAULT_FRAME_STYLE, 
                          title='OSSIE Talk')

        # most of the wx widgets will reside on the panel:
        panel = wx.Panel(self, -1)

        # Push to talk button
        self.TalkBtn = wx.Button(id=-1, label='Push to Talk',
                                    name='TalkBtn', 
                                    parent=panel, 
                                    size=talk_btn_size)
        self.TalkBtn.SetFont(wx.Font(16, wx.SWISS, wx.NORMAL, 
                                     wx.BOLD, False))
        self.TalkBtn.Bind(wx.EVT_LEFT_DOWN, self.OnStartTalk, id=-1)
        self.TalkBtn.Bind(wx.EVT_LEFT_UP, self.OnStopTalk, id=-1)

        # send text message button
        #self.TextBtn = wx.Button(id=-1, label='Send Text',
        #                            name='TextBtn', 
        #                            parent=panel, 
        #                            size=text_btn_size)
        #self.TextBtn.SetFont(wx.Font(16, wx.SWISS, wx.NORMAL, 
        #                                wx.BOLD, False))
        #self.TextBtn.Bind(wx.EVT_LEFT_UP, self.OnSendText, id=-1)

        # Text editors:
        #self.rxTextEditor = wx.TextCtrl(id=-1,
        #                                name=u'rxTextEditor', 
        #                                parent=panel, 
        #                                size = rx_text_box_size, 
        #                                style= wx.TE_MULTILINE, 
        #                                value=u'')
        #
        #self.textEditor = wx.TextCtrl(id=-1,
        #                              name=u'textEditor', 
        #                              parent=panel, 
        #                              size=text_edit_size, 
        #                              style=wx.TE_PROCESS_ENTER, 
        #                              value=u'')
        #self.textEditor.Bind(wx.EVT_TEXT_ENTER, self.OnSendText, id=-1)

        # sizer grid:
        sizer = wx.FlexGridSizer(cols=1, hgap=6, vgap = 6)
        #sizer.AddMany([self.TalkBtn, 
        #               self.rxTextEditor, 
        #               self.textEditor, 
        #               self.TextBtn])
        sizer.AddMany([self.TalkBtn])                       
        panel.SetSizer(sizer)



    def OnStartTalk(self,event):
        self.orb_ref.OSSIETalk_Obj.talk_flag = True
        event.Skip()

 
    def OnStopTalk(self,event):
        self.orb_ref.OSSIETalk_Obj.talk_flag = False
        event.Skip()

    #def OnSendText(self, event):
    #    text=''
    #	text =copy.copy(str(self.textEditor.GetLineText(0)))
    #    self.textEditor.Clear()
    #    if len(text)>0:
    #	        self.orb_ref.OSSIETalk_Obj.tx_work_mod.SendTextData(text)
    #            updatedtext=str(self.rxTextEditor.GetValue())
    #            updatedtext+='Snd('+strftime("%H:%M:%S",localtime()) + '):'+text+'\n'
    #            self.rxTextEditor.Clear()
    #            self.rxTextEditor.write(updatedtext)
    #    event.Skip()

    #def DisplayText(self, text):
    #    locker = wx.MutexGuiLocker()
    #    updatedtext=str(self.rxTextEditor.GetValue())
    #    updatedtext+='Rcv('+strftime("%H:%M:%S",localtime()) + '):'+copy.copy(text)+'\n'
    #    self.rxTextEditor.Clear()
    #    try:
    #        self.rxTextEditor.write(updatedtext)
    #    except:
    #        print "WARNING! OSSIETalk got incompatible ASCII symbols!"

    #def OnClearTextHistory(self,event):
    #    self.rxTextEditor.Clear()
    #    event.Skip()

    def OnFileExit(self, event):
        '''This is what will happen when you select File -> Exit 
           in the menu bar'''
        self.Close()      #close the frame
  
    def OnHelpAbout(self, event):
        '''Stuff that gets displayed when you select Help -> About in 
           the menu bar'''
        from wx.lib.dialogs import ScrolledMessageDialog
        about = ScrolledMessageDialog(self, "OSSIE Talk \nOSSIE \nA product of Wireless@VT.", 
                                      "About...")
        about.ShowModal()


    def OnCloseWindow(self,event):
        if hasattr(self.parent, 'removeToolFrame'):
            self.parent.removeToolFrame(self)
        self = None
        event.Skip()

    def __del__(self):
        pass


