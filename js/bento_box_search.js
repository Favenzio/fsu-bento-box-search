/* 
 * Bento Box Search Javascript functions to retrieve and display results
 * from the different Search Source Nodes.
 */

/* Fixed Header */
(function ($) {  
  Drupal.behaviors.bento_box_search = {
    attach: function (context, settings) {            
     // All our js code here
     $.each(Drupal.settings.bento_box_search, function(node_id, bento_query_url) {
       $.get(bento_query_url, function( results_html ) {
         $( "#" + node_id).append('<p>' + results_html + '</p>');
       });
     });
     // end our js code
   }
 };})(jQuery);


