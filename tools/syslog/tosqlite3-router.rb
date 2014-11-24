#!/usr/bin/ruby 

require 'rubygems'
require 'sqlite3'

chmode = true

ARGF.each do |l|

  msg = l.split(%r{\s*\|\s*},10) # "10" limit allows user-supplied messages to contain "|" delimiter
  timestamp = msg[0].to_s.strip
  hostname  = msg[1].to_s.strip
  hostaddr  = msg[2].to_s.strip
  severity  = msg[3].to_s.strip
  category  = msg[4].to_s.strip
  optappname= msg[5].to_s.strip
  optprocid = msg[6].to_s.strip
  optrunev  = msg[7].to_s.strip
  softmod   = msg[8].to_s.strip
  message   = msg[9].to_s.strip
  
  # Begin sqlite3 
  begin

    db = SQLite3::Database.open "/var/log/local0/messageFacilityLog.db"
    if chmode
      # allow others to read database
      File.chmod( 0644, "/var/log/local0/messageFacilityLog.db" )
      chmode = false
    end
    
    # Create header table 
    db.execute( "CREATE TABLE IF NOT EXISTS MessageHeaders(
    Timestamp TEXT, 
    Hostname TEXT, 
    Hostaddress TEXT,
    Severity TEXT,
    Category TEXT,
    AppName TEXT,
    ProcessId TEXT,
    RunEventNo TEXT,
    ModuleName TEXT
    )" )
    
    db.execute( "INSERT INTO MessageHeaders 
    (Timestamp, Hostname, Hostaddress, Severity, Category, AppName, ProcessId, RunEventNo, ModuleName )
     VALUES( ?,?,?,?,?,?,?,?,? )",
                [ timestamp, hostname, hostaddr, severity, category, optappname, optprocid, optrunev, softmod ] )
    
    hid = db.last_insert_row_id

    timing_delim_present = message.index('>') 
    mem_delim_present    = message.index(":")
    
    # > delimiter present for TimeEvent TimeModule
    if timing_delim_present && message.match("Time(.*)>")
      
      msg_type = message.slice(0...timing_delim_present)
      
      payload=message.slice((timing_delim_present+1)..(message.length)).split(%r{\s})
      run = -1
      srun = -1
      event = -1
      time = -1
      
      if msg_type.eql?("TimeEvent")
        
        payload.each_with_index do |e,i|
          if e.eql?("run:")
            run = payload[i+1]
          elsif e.eql?("subRun:")
            srun = payload[i+1]
          elsif e.eql?("event:")
            event = payload[i+1]
            time  = payload[i+2]
            break
          end
        end # TimeEvent loop
        
        # TimeEvent db
        db.execute( "CREATE TABLE IF NOT EXISTS TimeEvent(
        HeaderId INTEGER,
        Run    INTEGER,
        Subrun INTEGER,
        Event  INTEGER,
        Time   FLOAT
        )" )
        
        db.execute( "INSERT INTO TimeEvent(HeaderId,Run,Subrun,Event,Time) 
                     VALUES( ?,?,?,?,? )",
                    [hid,run,srun,event,time] )

        #end TimeEvent block
      elsif msg_type.eql?("TimeModule") 
        
        modulelabel= String.new
        modulename = String.new
        
        payload.each_with_index do |e,i|
          if e.eql?("run:")
            run = payload[i+1]
          elsif e.eql?("subRun:")
            srun = payload[i+1]
          elsif e.eql?("event:")
            event = payload[i+1]
            modulelabel = payload[i+2]
            modulename  = payload[i+3]
            time  = payload[i+4]
            break
          end 
        end #TimeModule loop
        
        #TimeModule db
        db.execute( "CREATE TABLE IF NOT EXISTS TimeModule(
        HeaderId INTEGER,
        Run    INTEGER,
        Subrun INTEGER,
        Event  INTEGER,
        ModuleLabel TEXT,
        ModuleName TEXT,
        Time   FLOAT 
        )" )
        
        db.execute( "INSERT INTO TimeModule(HeaderId,Run,Subrun,Event,ModuleLabel,ModuleName,Time) 
                     Values(?,?,?,?,?,?,?)",
                    [hid,run,srun,event,modulelabel,modulename,time] )
        
      end # TimeModule block

    elsif mem_delim_present && message.match("MemoryCheck:")

      type       = String.new
      modulename = String.new
      modulelabel= String.new
      vsize      = -1
      vsizedelta = -1
      rss        = -1
      rssdelta   = -1
      remainder  = String.new

      payload=message.slice((mem_delim_present+1)..(message.length) ).split(%r{\s|:})

      payload.each_with_index do |e,i|
        if i.eql?(1)
          type       = payload[i]
          modulename = payload[i+1]
          modulelabel= payload[i+2]
        elsif e.eql?("VSIZE")
          vsize      = payload[i+1]
          vsizedelta = payload[i+2] 
        elsif e.eql?("RSS")
          rss       = payload[i+1]
          rssdelta  = payload[i+2] 
          remainder = payload[i+3] #extra printout if LINUX compile-time flag activated
          break
        end
      end #MemoryCheck loop

      #MemoryCheck db
      db.execute( "CREATE TABLE IF NOT EXISTS MemoryCheck(
         HeaderId INTEGER,
         Type TEXT,
         ModuleName TEXT,
         ModuleLabel TEXT,
         Vsize FLOAT,
         VsizeDelta FLOAT,
         RSS FLOAT,
         RSSDelta FLOAT,
         Remainder TEXT 
         )" )
      
      db.execute( "INSERT INTO MemoryCheck(HeaderId,Type,ModuleName,ModuleLabel,Vsize,VsizeDelta,RSS,RSSDelta,Remainder) 
                   Values(?,?,?,?,?,?,?,?,?)",
                  [hid,type,modulename,modulelabel,vsize,vsizedelta,rss,rssdelta,remainder] )
      
    else # everything else  

      # Create general message table
      db.execute( "CREATE TABLE IF NOT EXISTS UserMessages(
      HeaderId INTEGER,
      Message TEXT
      )" )
      
      db.execute( "INSERT INTO UserMessages(HeaderId, Message) VALUES( ?,? )",
                  [ hid, message ] ) 
    end

  rescue SQLite3::Exception => e
    errOutput = File.open( "/var/log/local0/err.log","a+")
    errOutput << "Exception occurred\n"
    errOutput.puts e
    errOutput.close
    File.chmod( 0644, "/var/log/local0/err.log" )

  ensure

    db.close if db

  end

end
