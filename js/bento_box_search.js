/* 
 * Bento Box Search Javascript functions to retrieve and display results
 * from the different Search Source Nodes.
 */

/* Fixed Header */
(function ($) {  
  Drupal.behaviors.bento_box_search = {
    attach: function (context, settings) {            
     // All our js code here
     $.each(Drupal.settings.bento_box_search, function(index, value) {
       $( "#" + index).append("<p>Test</p>"); 
     });
     // end our js code
   }
 };})(jQuery);


