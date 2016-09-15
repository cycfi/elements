/*=================================================================================================
   Copyright (c) 2016 Joel de Guzman

   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.
   http://creativecommons.org/licenses/by-sa/4.0/
=================================================================================================*/
#include <photon/view.hpp>
#include <infinity/application.hpp>

namespace client
{
   void  init(photon::view& view_)
   {
      view_.maintain_aspect(true);
      view_.app = std::make_shared<infinity::application>(view_);
   }
}
