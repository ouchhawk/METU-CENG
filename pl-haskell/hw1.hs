module HW1 where

--Data
------

type RealName = String
type UserName = String
type GroupName = String
type Message = String

data Post    = Post UserName Message deriving (Show, Eq)
data To 	 = UserID UserName | GroupID GroupName deriving (Show, Eq)
data User    = User UserName RealName [UserName] [Post] deriving (Show, Eq)
data Group   = Group GroupName [UserName] [Post] deriving (Show, Eq)
data DB		 = DB [User] [Group] deriving (Show, Eq)

--1. Commands

newUser      :: DB -> User -> DB
addFriend    :: DB -> UserName -> UserName -> DB
sendPost 	 :: DB -> UserName -> Message -> [To] -> DB
newGroup 	 :: DB -> GroupName -> DB
addMember 	 :: DB -> GroupName -> UserName -> DB
removeMember :: DB -> GroupName -> UserName -> DB

--2. Queries

getFriendNames :: DB -> UserName -> [RealName]
getPosts 	   :: DB -> To -> [Post]
listGroups 	   :: DB -> UserName -> [Group]
suggestFriends :: DB -> User -> Int -> [User]

---- IMPLEMENTATIONS ----

---YARDIMCI------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

elo::User->DB->DB
elo a (DB b c) = (DB (a:b) c)

elo1::UserName->DB->DB
elo1 a (DB b ((Group c d f):gr)) = (DB b ((Group c (a:d) f):gr))

elo2::Group->DB->DB
elo2 a (DB b c) = (DB b (a:c))



--checks if user already exists,returns bool
check :: [User] -> User -> Bool
check [] x1 = False
check (x:xs) x1 = if (x /= x1) && (check xs x1) then False 
				  else True
-----------------------------------------------------------------------------------
checkgroup :: [Group] -> GroupName -> Bool
checkgroup [] x1 = False
checkgroup (Group gn unl pl:xs) x1 = if (gn /= x1) && (checkgroup xs x1) then False 
					else True
------------------------------------------------------------------------sendtoOne
sendtoOne :: DB -> UserName -> Message -> UserName -> DB
sendtoOne  (DB [] g) sender message x = (DB [] g)
sendtoOne (DB ((User u1 a b userpost):uxs) g) sender message x = if (u1==x) then (if((elem (Post sender message) userpost)==False) then (DB ((User u1 a b ((Post sender message):userpost)):uxs) g) else (DB ((User u1 a b userpost):uxs) g))
																 else elo (User u1 a b userpost) (sendtoOne (DB uxs g) sender message x)

------------------------------------------------------------------------sendtoAll
sendtoAll :: DB -> UserName -> Message -> [UserName] -> DB
sendtoAll db sender message [] = db
sendtoAll db sender message (x:xs)= sendtoAll (sendtoOne db sender message x) sender message xs

------------------------------------------------------------------------addback the friend
addback :: DB -> UserName -> UserName -> DB
addback (DB ((User username realname friendlist post):xs) g) u1 u2 = if (username == u2) then (DB ((User username realname (u1:friendlist) post):xs) g)
																	else elo (User username realname friendlist post) (addback (DB xs g) u1 u2)
------------------------------------------------------------------------checkfriendlist
checkfriendlist::[UserName]->UserName->Bool --check if friend exist in friendlist
checkfriendlist [] f = True
checkfriendlist (f1:fr) f = if (f1==f) then False
							else checkfriendlist fr f 
------------------------------------------------------------------------realist
realist::DB->DB->[UserName]->[UserName]->[UserName]
realist (DB ((User un rn fl pl):ur) g) db [] t = t
realist (DB ((User un rn fl pl):ur) g) db (x:xs) t = if (un==x) 
													 then realist db db xs (rn:t) 
												     else (realist (DB ur g) db (x:xs) t)
------------------------------------------------------------------------findtargetsfriendlist
findfl::DB->UserName->[UserName]
findfl (DB ((User un rn fl pl):ur) gl) target = if (un==target) then fl
												else findfl (DB ur gl) target
------------------------------------------------------------------------
listgroupnames::DB->UserName->[Group]->[Group]
listgroupnames (DB user []) un t = t
listgroupnames (DB user ((Group gn [] gpl):restofgroups)) un t = listgroupnames (DB user restofgroups) un t
listgroupnames (DB user ((Group gn (u1:ur) gpl):restofgroups)) un1 t = if (u1==un1) 
																	   then listgroupnames (DB user restofgroups) un1 ((Group gn (u1:ur) gpl):t)
																	   else listgroupnames (DB user ((Group gn ur gpl):restofgroups)) un1 t
------------------------------------------------------------------------ arkadaslarinin nicknameleri olan liste donur
friends::DB->User->[UserName]
friends (DB ((User u1 rn fl pl):ur) g) un = if ((User u1 rn fl pl)==un) then fl
												   else friends (DB ur g) un
------------------------------------------------------------------------friends of friends (nickler listesi)
fof::DB->DB->[UserName]->[UserName]->[UserName]
fof (DB ((User u1 rn fl pl):ur) g) db [] t = t 
fof (DB ((User u1 rn fl pl):ur) g) db (f1:fr) t = if (u1==f1) then ( fof db db fr (fl++t) )
												  else fof (DB ur g) db (f1:fr) t
------------------------------------------------------------------------count
count::[UserName]->UserName->Int->Int
count [] un x = x
count (u1:ur) un x = if (u1==un) then count ur un (x+1)
					 else count ur un x
-------------------------------------------------------------------------not repeated
repeated :: [UserName] -> Bool
repeated [] = False
repeated [_] = False
repeated (h:t) = if elem h t 
				 then True
                 else repeated t
------------------------------------------------------------------------suggest
suggest::[UserName]->Int->[UserName]->[UserName]
suggest [] x t = t
suggest (u1:ur) x t = if ((count (u1:ur) u1 0)>=x) && repeated (u1:ur)  then suggest ur x (u1:t) ---------------buyuk ve ya beraberse !!!!!!!!!!!!!
					  else suggest ur x t
-------------------------------------------------------------------------[UserName](nick) to [User]
convert::DB->DB->[UserName]->[User]->[User]
convert (DB ((User u1 rn fl pl):ur) g) db [] ul = ul
convert (DB ((User u1 rn fl pl):ur) g) db (un1:unr) ul = if (u1==un1) then convert db db unr ((User u1 rn fl pl):ul)
														 else convert (DB ur g) db (un1:unr) ul

-------------------------------------------------------------------------deleteowner's name
delself::[UserName]->UserName->[UserName]->[UserName]
delself [] username t = t
delself (u1:ur) username t = if (u1/=username) then delself ur username (u1:t)
							 else delself ur username t

---MAIN-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------ new User
newUser (DB [] y) p = (DB (p:[]) y) 
newUser (DB x y) p = if check x p then (DB (p:x) y)
					 else (DB x y)
------------------------------------------------------------------------addFriend

addFriend (DB ((User u1 realname fl postlist):xs) y) p1 p2 = if ((u1 == p1) && (checkfriendlist fl p2)) 
															 then addback (DB ((User u1 realname (p2:fl) postlist):xs) y) p1 p2
 															 else if ((u1 == p2) && (checkfriendlist fl p1))
 															 then addback (DB ((User u1 realname (p1:fl) postlist):xs) y) p2 p1
 															 else elo (User u1 realname fl postlist) (addFriend (DB xs y) p1 p2)

----------------------------------------sendPost

mini::DB->UserName->Message->To->DB
mini (DB [] g) sender message (UserID ugetter) = (DB [] g)
mini (DB ((User u1 rn un userpost):uxs) g) sender message (UserID ugetter) = if (u1==ugetter) then if(elem (Post sender message) userpost)==False then (DB ((User u1 rn un ((Post sender message):userpost)):uxs) g) else (DB ((User u1 rn un userpost):uxs) g)
																			else elo (User u1 rn un userpost) (mini (DB uxs g) sender message (UserID ugetter))
mini (DB u []) sender message (GroupID ggetter) = (DB u [])
mini (DB u ((Group g2 (firstuser:restuser) grouppost):gxs)) sender message (GroupID ggetter) = if (g2==ggetter) then if(elem (Post sender message) grouppost)==False then sendtoAll (DB u ((Group g2 (firstuser:restuser) ((Post sender message):grouppost)):gxs)) sender message (firstuser:restuser) else sendtoAll (DB u ((Group g2 (firstuser:restuser) grouppost):gxs)) sender message (firstuser:restuser)  --sendtoAll (DB u ((Group g2 (firstuser:restuser) ((Post sender message):grouppost)):gxs)) sender message (firstuser:restuser)
																			else elo2 (Group g2 (firstuser:restuser) grouppost) (mini (DB u gxs) sender message (GroupID ggetter))

many::DB->UserName->Message->[To]->DB
many db sender message [] = db
many db sender message (x:xs) = many (mini db sender message x) sender message xs
								

sendPost db sender message x = (many db sender message x)

------------------------------------------------------------------------newGroup
newGroup (DB x []) p = (DB x ((Group p [] []):[])) 
newGroup (DB x y) p = if checkgroup y p then (DB x ((Group p [] []):y))
					 else (DB x y)
------------------------------------------------------------------------addMember
--addMember (DB ((User u a b c):ur) []) groupname username = (DB ((User u a b c):ur) [Group groupname [username] []]] )    -- oyle bi grup yoksa (olmaya bilir boyle ihtimal)
addMember (DB ((User u a b c):ur) ((Group g gl w):gr)) groupname username=if (u==username) 
															              then (if (g==groupname) then (DB ((User u a b c):ur) ((Group g (username:gl) w):gr) ) else addMember (DB ((User u a b c):ur) gr) groupname username ) 
															              else addMember (DB ur ((Group g gl w):gr)) groupname username  --if-i parantez icine almak oluyosa


------------------------------------------------------------------------removeMember

removeMember (DB user ((Group gn (u1:ur) gpl):xs)) groupname username = if (gn == groupname)
	then (if (elem username (u1:ur) )  then (if(u1==username) then (DB user ((Group gn ur gpl):xs)) else elo1 u1 (removeMember (DB user ((Group gn ur gpl):xs)) groupname username) ) else (DB user ((Group gn (u1:ur) gpl):xs)) )--grupta degilse kalsin ayni
	else elo2 (Group gn (u1:ur) gpl) (removeMember (DB user xs) groupname username)

------------------------------------------------------------------------getFriendNames

getFriendNames db target = (realist db db (findfl db target) [])

------------------------------------------------------------------------getPosts

getPosts (DB ((User un rn fl pl):ur) g) (UserID u) = if (un==u) then pl
								 else getPosts (DB ur g) (UserID u)
getPosts (DB user ((Group gn ul gpl):xs)) (GroupID g) = if (g==gn) then gpl
								 else getPosts (DB user xs) (GroupID g)

------------------------------------------------------------------------listGroups

listGroups db un =  (listgroupnames db un [])

------------------------------------------------------------------------suggestFriends

suggestFriends db (User u1 realname fl pl) rakam = convert db db (delself (suggest (fof db db (friends db (User u1 realname fl pl)) []) rakam []) u1 []) []