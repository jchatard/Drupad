<?php // $Id$

function drupad_users() {

  // Switch on the verb/action to take
  switch (arg(3)) {
    case 'block':
      $data = drupad_users_block_uid(arg(2));
      break;
    case 'active':
      $data = drupad_users_active_uid(arg(2));
      break;
    case 'delete':
      $data = drupad_users_delete_uid(arg(2));
      break;
    default:
      $data = drupad_users_list();
      break;
  }

  drupad_serve_response($data);
}

function drupad_users_list() {
  if (!user_access('administer users')) {
    return array();
  }

  if (isset($_GET['status']) && $_GET['status'] != '' && is_numeric($_GET['status'])) {
    $status = (int) $_GET['status'];
  }
  else {
    $status = 1;
  }

  $users = array();

  $query = db_select('users', 'u')->extend('PagerDefault');
  $query->condition('u.uid', 0, '<>')
        ->fields('u', array('uid', 'name', 'status', 'mail', 'picture', 'created', 'login'))
        ->orderBy('created', 'DESC')
        ->limit(50);

  $result = $query->execute();

  $users = array();

  foreach ($result as $user) {
    if (!is_null($user->picture)) {
      $file = file_load($user->picture);
      $user->picture = isset($file->uri) ? file_create_url($file->uri) : NULL;
    }
    $users[] = $user;
  }

  $data['content'] = $users;

  global $pager_total_items;
  $data['meta']['total_count'] = $pager_total_items[0];
  return $data;
}

function drupad_users_block_uid($uid) {
  if (!user_access('administer users')) {
    return array();
  }

  if ($uid == 1 || $uid == 0) {
    $data = array(
      'meta' => array('uid' => $uid, 'was_blocked' => FALSE),
      'content' => array(),
    );
    return $data;
  }

  $account = user_load((int) $uid);
  // Skip blocking user if they are already blocked.
  if ($account !== FALSE && $account->status == 1) {
    user_save($account, array('status' => 0));
  }
  $data = array(
    'meta' => array(
      'uid' => $uid,
      'change' => 'status',
      'value' => 0,
    ),
    'content' => array(),
  );
  return $data;
}

function drupad_users_active_uid($uid) {
  if (!user_access('administer users')) {
    return array();
  }

  if ($uid == 1) {
    $data = array(
      'meta' => array('uid' => $uid, 'was_actived' => FALSE),
      'content' => array(),
    );
    return $data;
  }

  $account = user_load((int) $uid);
  // Skip blocking user if they are already blocked.
  if ($account !== FALSE && $account->status == 0) {
    user_save($account, array('status' => 1));
  }
  $data = array(
    'meta' => array(
      'uid' => $uid,
      'change' => 'status',
      'value' => 1,
    ),
    'content' => array(),
  );
  return $data;
}

function drupad_users_delete_uid($uid) {
  if (!user_access('administer users')) {
    return array();
  }

  if ($uid == 1 || $uid == 0) {
    $data = array(
      'meta' => array(
        'uid' => $uid,
        'change' => 'delete',
        'value' => 0,
      ),
      'content' => array(),
    );
    return $data;
  }

  $account = user_load((int) $uid);

  if ($account !== FALSE) {
    user_delete($uid);
  }

  $data = array(
    'meta' => array(
      'uid' => $uid,
      'change' => 'delete',
      'value' => 1,
    ),
    'content' => array(),
  );

  return $data;
}