/* 
 * Bento Box Search Javascript functions to retrieve and display results
 * from the different Search Source Nodes.
 */

/* Fixed Header */
(function ($) {  
  Drupal.behaviors.bento_box_search = {
    attach: function (context, settings) {            
     // All our js code here
     $.each(Drupal.settings.bento_box_search, function(index, id_keywords) {
       var temp_url = id_keywords.split("|"); 
       var bento_query_url = '/bento_search_query/' + temp_url[0] + '/' + temp_url[1];
       $('#edit-bento-search-input').val(temp_url[1]);
       $.get(bento_query_url, function( results_html ) {
         $('#' + temp_url[0]).append(results_html);
         $('#ajax-loader-' + temp_url[0]).empty();
         $('.bento-search-system-container').height(300);
       });
    
     });
     // end our js code
   }
 };})(jQuery);


