<?php


/**
 * @file
 * Admin functions for uc_cart_bonus
 *
 */


/**
 * Define the affiliate users listing
 */
function uc_cart_bonus_admin_manage() {

  $output = "";

  // setup the table
  $header = array(
    array('data' => t('Rule Name'), 'field' => 'u.rule_name'),
    array('data' => t('Status'), 'field' => 'u.status'),

  );


  // define the query
  $sql = "SELECT u.* FROM {uc_cart_bonus} u";

  $result = db_query($sql);

  while ($bonus = db_fetch_object($result)) {
    $rows[] = array(
      $bonus->rule_name,
      l(t('Edit'), 'admin/store/settings/cart_bonus/' . $bonus->bid . '/edit') . ' '.l(t('Delete'), 'admin/store/settings/cart_bonus/' . $bonus->bid . '/delete')
    );
  }

  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, 50, 0);

  return $output;
}

/**
 * Form builder for product attributes.
 *
 * @see uc_attribute_form_submit()
 * @ingroup forms
 */
function uc_cart_bonus_form($form_state, $attribute = NULL) {
  if (!empty($attribute)) {
    if (is_numeric($attribute)) {
      $attribute = _uc_cart_bonus_get_rule($attribute);
    }
  }
  // If an attribute specified, add its ID as a hidden value.
  if (!empty($attribute)) {
    $form['bid'] = array('#type' => 'hidden', '#value' => $attribute->bid);
    drupal_set_title(t('Edit cart bonus rule: %name', array('%name' => $attribute->rule_name)));
  }

  $options = _uc_cart_bonus_product_list();

  $form['rule_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('The name of the cart bonus used in administrative forms'),
    '#default_value' => $attribute->rule_name,
    '#required' => TRUE,
  );

  $form['rule_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Message text'),
    '#required' => TRUE,
    '#description' => t('Enter the text that will appear along with their cart entry'),
    '#default_value' => isset($attribute->rule_text) ? $attribute->rule_text : '',
    '#maxlength' => 255,
  );

  $form['bonus'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bonus rule'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#attributes' => array('class' => 'product-field'),
  );

  $form['bonus']['cart_node_trigger'] = array(
    '#type' => 'select',
    '#title' => t('Products this rule triggers on'),
    '#description' => t('Controls which products activate this rule'),
    '#options' => $options,
    '#multiple' => TRUE,
    '#size' => 10,
    '#required' => TRUE,
    '#default_value' => $attribute->cart_node_trigger,
  );

  $form['bonus']['cart_node_trigger_op'] = array(
    '#type' => 'select',
    '#title' => t('Rule operation'),
    '#description' => t('Does this rule require ANY or ALL of the selected products?'),
    '#options' => array('any' => 'ANY', 'all' => 'ALL'),
    '#required' => TRUE,
    '#default_value' => $attribute->cart_node_trigger_op,
  );

  $form['bonus']['min_required'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum items required of each'),
    '#description' => t('Minimum required of each'),
    '#default_value' => $attribute->min_required,
    '#size' => 5,
    '#required' => TRUE,
  );


  $form['product'] = array(
    '#type' => 'fieldset',
    '#title' => t('Products added to Cart'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#attributes' => array('class' => 'product-field'),
  );

  $form['product']['cart_node_add'] = array(
    '#type' => 'select',
    '#title' => t('Products to add to their cart'),
    '#description' => t('Which products to add to their cart'),
    '#options' => $options,
    '#multiple' => TRUE,
    '#required' => TRUE,
    '#size' => 10,
    '#default_value' => $attribute->cart_node_add,
  );

  $form['product']['qty'] = array(
    '#type' => 'textfield',
    '#title' => t('Quantity to add to cart'),
    '#description' => t('Quantity to add to cart'),
    '#default_value' => $attribute->qty,
    '#size' => 5,
    '#required' => TRUE,
  );

  $form['product']['cart_node_discount'] = array(
    '#type' => 'textfield',
    '#title' => t('Discount amount'),
    '#description' => t('The amount in % to discount the products that are added to their cart'),
    '#default_value' => $attribute->cart_node_discount,
    '#size' => 5,
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#suffix' => l(t('Cancel'), 'admin/store/cart_bonus'),
  );


  return $form;
}

function _uc_cart_bonus_product_list() {

  $products = array();

  $result = db_query("SELECT title, nid FROM {node} where type='product'");
  while ($product = db_fetch_object($result)) {
    $products[$product->nid] = $product->title;
  }

  return $products;

}

function uc_cart_bonus_form_submit($form, &$form_state) {

  if (isset($form_state['values']['bid'])) {

    db_query("UPDATE {uc_cart_bonus} set rule_name ='%s', rule_text ='%s', min_required =%d, cart_node_trigger ='%s', cart_node_trigger_op ='%s', cart_node_add ='%s', cart_node_discount ='%s', qty =%d where bid=%d",
      $form_state['values']['rule_name'],
      $form_state['values']['rule_text'],
      $form_state['values']['min_required'],
      serialize($form_state['values']['cart_node_trigger']),
      $form_state['values']['cart_node_trigger_op'],
      serialize($form_state['values']['cart_node_add']),
      $form_state['values']['cart_node_discount'],
      $form_state['values']['qty'],
      $form_state['values']['bid']);

  } else {

  db_query("INSERT INTO {uc_cart_bonus} (rule_name, rule_text, min_required, cart_node_trigger, cart_node_trigger_op, cart_node_add, cart_node_discount, qty )
    VALUES ('%s', '%s', %d, '%s', '%s', '%s','%s', %d)",
    $form_state['values']['rule_name'],
    $form_state['values']['rule_text'],
    $form_state['values']['min_required'],
    serialize($form_state['values']['cart_node_trigger']),
    $form_state['values']['cart_node_trigger_op'],
    serialize($form_state['values']['cart_node_add']),
    $form_state['values']['cart_node_discount'],
    $form_state['values']['qty']);

  }

  drupal_set_message('Cart Bonus rule updated');
  drupal_goto('admin/store/cart_bonus');
}

function uc_cart_bonus_delete_form($form_state, $attribute = NULL) {
  $form = array();
  if (!empty($attribute)) {
    if (is_numeric($attribute)) {
      $attribute = _uc_cart_bonus_get_rule($attribute);
    }
  }
  // If an attribute specified, add its ID as a hidden value.
  if (!empty($attribute)) {
    $form['bid'] = array('#type' => 'hidden', '#value' => $attribute->bid);
    drupal_set_title(t('Delete cart bonus rule: %name', array('%name' => $attribute->rule_name)));
  }

  $form = confirm_form(
    $form, t('Delete cart bonus rule: %name', array('%name' => $attribute->rule_name)),
    'admin/store/cart_bonus',
    t('Deleting will permently remove it from the system'),
    t('Delete Cart Bonus configuration'), t('Cancel')
  );
  return $form;
}




function uc_cart_bonus_delete_form_submit($form, &$form_state) {

  if (isset($form_state['values']['bid'])) {
    db_query("DELETE FROM {uc_cart_bonus} where bid = %d", $form_state['values']['bid']);
  }

  drupal_set_message('Cart Bonus rule deleted');
  drupal_goto('admin/store/cart_bonus');
}
