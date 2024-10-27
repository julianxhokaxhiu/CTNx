#*****************************************************************************#
#    Copyright (C) 2024 Julian Xhokaxhiu                                      #
#                                                                             #
#    This file is part of CTNx                                                #
#                                                                             #
#    CTNx is free software: you can redistribute it and\or modify             #
#    it under the terms of the GNU General Public License as published by     #
#    the Free Software Foundation, either version 3 of the License            #
#                                                                             #
#    CTNx is distributed in the hope that it will be useful,                  #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of           #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
#    GNU General Public License for more details.                             #
#*****************************************************************************#

# $downloadUrl = "https://github.com/julianxhokaxhiu/CTNx/releases/latest"

# if ($env:_IS_BUILD_CANARY -eq "true") {
#   $downloadUrl = "https://github.com/julianxhokaxhiu/CTNx/releases/tag/canary"
# }

# # Initial template from https://discohook.org/
# $discordPost = @"
# {
#   "username": "CTNx",
#   "avatar_url": "https://github.com/julianxhokaxhiu/CTNx/raw/master/.logo/logo_whitebg.png",
#   "content": "Release **${env:_RELEASE_VERSION}** has just been published!\n\nDownload Url: ${downloadUrl}\n\nIf you find something broken or unexpected, feel free to check existing ones first here https://github.com/julianxhokaxhiu/CTNx/issues.\nIf non existing, then report your issue here https://github.com/julianxhokaxhiu/CTNx/issues/new.\n\nThank you for using CTNx!",
#   "embeds": [
#     {
#       "title": "How to install",
#       "description": "Feel free to follow instructions at this link, depending on which version of the game you own: https://github.com/julianxhokaxhiu/CTNx/blob/master/docs/how_to_install.md",
#       "color": 7506394
#     },
#     {
#       "title": "CTNx is FOSS Software!",
#       "description": "CTNx is released under GPLv3 license. More info here: https://github.com/julianxhokaxhiu/CTNx#license",
#       "color": 15746887
#     }
#   ]
# }
# "@

# Invoke-RestMethod -Uri $env:_MAP_CTNx_QHIMM_CT -ContentType "application/json" -Method Post -Body $discordPost
# Invoke-RestMethod -Uri $env:_MAP_CTNx_TSUNAMODS_CT -ContentType "application/json" -Method Post -Body $discordPost
